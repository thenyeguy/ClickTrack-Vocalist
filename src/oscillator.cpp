#include <iostream>
#include <cmath>
#include "portaudio_wrapper.h"
#include "oscillator.h"

using namespace Oscillators;
const double PI = 4.0*atan(1.0);

Oscillator::Oscillator(float in_freq)
    : FilterGenerics::AudioGenerator(1), next_i(0), freq(in_freq), paused(false)
{}


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
    freq = in_freq;
}


void Oscillator::generate_outputs(SAMPLE** outputs)
{
    for(int i = 0; i < FilterGenerics::DEFAULT_BLOCK_SIZE; i++)
    {
        float next_t = ((float)next_i) / Portaudio::DEFAULT_SAMPLE_RATE;
        outputs[0][i] = paused ? 0.0 : f(next_t);
        next_i++;
    }
}


SinWave::SinWave(float in_freq)
    : Oscillator(in_freq) {}
float SinWave::f(float t)
{
    return sin(2*PI*freq*t);
}


SquareWave::SquareWave(float in_freq)
    : Oscillator(in_freq) {}
float SquareWave::f(float t)
{
    float tprime = fmod(t*freq, 1.0);
    if(tprime < .5)
        return 1.0;
    else
        return 0.0;
}


TriangleWave::TriangleWave(float in_freq)
    : Oscillator(in_freq) {}
float TriangleWave::f(float t)
{
    float tprime = fmod(t*freq, 1.0);

    return 2*(fabs(tprime - 0.5));
}
