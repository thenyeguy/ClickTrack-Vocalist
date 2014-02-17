#include <iostream>
#include <cmath>
#include <random>
#include "portaudio_wrapper.h"
#include "oscillator.h"

using namespace ClickTrack;

Oscillator::Oscillator(float in_freq)
    : AudioGenerator(1), phase(0.0), phase_inc(in_freq * 2*M_PI/SAMPLE_RATE), 
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


void Oscillator::set_freq(float in_freq)
{
    // Tracks the current phase to maintain phase during 
    freq = in_freq;
    phase_inc = freq * 2*M_PI/SAMPLE_RATE;
}


void Oscillator::generate_outputs(std::vector< std::vector<SAMPLE> >& outputs)
{
    for(int i = 0; i < FRAME_SIZE; i++)
    {
        // Generate this output
        outputs[0][i] = paused ? 0.0 : f();

        // Update the phase
        phase += phase_inc;
        if(phase > 2*M_PI) phase -= 2*M_PI;
    }
}


SinWave::SinWave(float in_freq) : Oscillator(in_freq) {};
float SinWave::f()
{
    return sin(phase);
}


WhiteNoise::WhiteNoise(float in_freq) : Oscillator(in_freq) {};
float WhiteNoise::f()
{
    int si = rand();
    float sf = ((float) si) / RAND_MAX;
    return 2*sf - 1;
}


SimpleSawWave::SimpleSawWave(float in_freq) : Oscillator(in_freq) {};
float SimpleSawWave::f()
{
    return phase/M_PI - 1.0;
}


SimpleSquareWave::SimpleSquareWave(float in_freq) : Oscillator(in_freq) {};
float SimpleSquareWave::f()
{
    if(phase < M_PI)
        return 1.0;
    else
        return -1.0;
}


SimpleTriangleWave::SimpleTriangleWave(float in_freq) : Oscillator(in_freq) {};
float SimpleTriangleWave::f()
{
    return 4*(fabs(phase/(2*M_PI) - 0.5)) - 1.0;
}


PolyBlepOscillator::PolyBlepOscillator(float in_freq) : Oscillator(in_freq) {};
float PolyBlepOscillator::polyBlepOffset(float t)
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


SawWave::SawWave(float in_freq) : PolyBlepOscillator(in_freq) {};
float SawWave::f()
{
    float out = 2.0*phase/(2*M_PI) - 1.0;

    // one discontinuity, at edge of saw
    out -= polyBlepOffset(phase/(2*M_PI));

    return out;
}


SquareWave::SquareWave(float in_freq) : PolyBlepOscillator(in_freq) {};
float SquareWave::f()
{
    float out;
    if(phase < M_PI)
        out = 1.0;
    else
        out = -1.0;

    // two discontinuities, at rising and falling edge
    out += polyBlepOffset(phase/(2*M_PI));
    out -= polyBlepOffset(fmod(phase/(2*M_PI) + 0.5, 1.0));

    return out;
}


TriangleWave::TriangleWave(float in_freq)
    : PolyBlepOscillator(in_freq), last_output(0.0) {};
float TriangleWave::f()
{
    // Compute a square wave signal
    float out;
    if(phase < M_PI)
        out = 1.0;
    else
        out = -1.0;

    // two discontinuities, at rising and falling edge
    out += polyBlepOffset(phase/(2*M_PI));
    out -= polyBlepOffset(fmod(phase/(2*M_PI) + 0.5, 1.0));

    // Perform leaky integration of a square wave
    out = phase_inc*out + (1-phase_inc)*last_output;
    last_output = out;
    return out;
}
