#include <iostream>
#include <cmath>
#include <random>
#include "portaudio_wrapper.h"
#include "oscillator.h"

using namespace ClickTrack;

Oscillator::Oscillator(float in_freq, OscMode in_mode)
    : AudioGenerator(1), scheduler(*this), last_output(0.0),
      phase(0.0), phase_inc(in_freq * 2*M_PI/SAMPLE_RATE), mode(in_mode),
      paused(false), freq(in_freq)
{}


bool Oscillator::is_paused()
{
    return paused;
}


void Oscillator::pause()
{
    paused = true;
}


void Oscillator::unpause()
{
    paused = false;
}


void Oscillator::set_mode(OscMode in_mode)
{
    mode = in_mode;
}


void Oscillator::set_freq(float in_freq, unsigned long time)
{
    if(time == 0)
        time = next_out_t;

    // Put the frequency in the payload and schedule the call
    float* payload = new float;
    *payload = in_freq;
    scheduler.schedule(time, Oscillator::set_freq_callback, payload);
}


void Oscillator::set_freq_callback(Oscillator& caller, void* payload)
{
    // Get the payload
    float* in_freq = (float*) payload;

    // Tracks the current phase to maintain phase during 
    caller.freq = *in_freq;
    caller.phase_inc = caller.freq * 2*M_PI/SAMPLE_RATE;

    // Release it
    delete in_freq;
}


void Oscillator::generate_outputs(std::vector< std::vector<SAMPLE> >& outputs)
{
    for(int i = 0; i < FRAME_SIZE; i++)
    {
        // Run event changes
        scheduler.run(next_out_t + i);

        // Generate this output
        outputs[0][i] = paused ? 0.0 : f();

        // Update the phase
        phase += phase_inc;
        if(phase > 2*M_PI) phase -= 2*M_PI;
    }
}


SAMPLE Oscillator::f()
{
    switch(mode)
    {
        case Sine:
        {
            return sin(phase);
        }

        case Saw:
        case BlepSaw:
        {
            float out = 2.0*phase/(2*M_PI) - 1.0;

            // one discontinuity, at edge of saw
            if(mode == BlepSaw)
                out -= polyBlepOffset(phase/(2*M_PI));

            return out;
        }

        case Square:
        case BlepSquare:
        {
            float out;
            if(phase < M_PI)
                out = 1.0;
            else
                out = -1.0;

            // two discontinuities, at rising and falling edge
            if(mode == BlepSquare)
            {
                out += polyBlepOffset(phase/(2*M_PI));
                out -= polyBlepOffset(fmod(phase/(2*M_PI) + 0.5, 1.0));
            }

            return out;
        }

        case Tri:
        case BlepTri:
        {
            // Compute a square wave signal
            float out;
            if(phase < M_PI)
                out = 1.0;
            else
                out = -1.0;

            // two discontinuities, at rising and falling edge
            if(mode == BlepTri)
            {
                out += polyBlepOffset(phase/(2*M_PI));
                out -= polyBlepOffset(fmod(phase/(2*M_PI) + 0.5, 1.0));
            }

            // Perform leaky integration of a square wave
            out = phase_inc*out + (1-phase_inc)*last_output;
            last_output = out;
            return out;
        }

        case WhiteNoise:
        {
            int si = rand();
            float sf = ((float) si) / RAND_MAX;
            return 2*sf - 1;
        }
    }
}


float Oscillator::polyBlepOffset(float t)
{
    float dt = phase_inc / (2*M_PI);
    if (t < dt)
    {
        t /= dt;
        return t+t - t*t - 1.0;
    }
    else if (t > 1.0 - dt)
    {
        t = (t - 1.0) / dt;
        return t*t + t+t + 1.0;
    }
    else
    {
        return 0.0;
    }
}
