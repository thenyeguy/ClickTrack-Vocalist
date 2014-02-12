#include <iostream>
#include <cmath>
#include <random>
#include "portaudio_wrapper.h"
#include "oscillator.h"

using namespace ClickTrack;

Oscillator::Oscillator(float in_freq)
    : AudioGenerator(1), phase(0.0), paused(false),
      freq(in_freq), phase_inc(freq * 2*M_PI/SAMPLE_RATE)
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


SinWave::SinWave(float in_freq)
    : Oscillator(in_freq) {}
float SinWave::f()
{
    return sin(phase);
}


SawWave::SawWave(float in_freq)
    : Oscillator(in_freq) {}
float SawWave::f()
{
    return phase/(2*M_PI);
}


SquareWave::SquareWave(float in_freq)
    : Oscillator(in_freq) {}
float SquareWave::f()
{
    if(phase < M_PI)
        return 1.0;
    else
        return 0.0;
}


TriangleWave::TriangleWave(float in_freq)
    : Oscillator(in_freq) {}
float TriangleWave::f()
{
    return 2*(fabs(phase/(2*M_PI) - 0.5));
}


WhiteNoise::WhiteNoise(float in_freq)
    : Oscillator(in_freq) {}
float WhiteNoise::f()
{
    int si = rand();
    float sf = ((float) si) / RAND_MAX;
    return 2*sf - 1;
}
