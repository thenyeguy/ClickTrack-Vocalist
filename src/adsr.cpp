#include <iostream>
#include "adsr.h"

using namespace std;
using namespace FilterGenerics;
using namespace Filters;


ADSRFilter::ADSRFilter(float in_attack_time, float in_decay_time,
                       float in_sustain_level, float in_release_time,
                       OutputChannel** in_input_channels,
                       unsigned in_num_channels)
    : AudioFilter(in_num_channels, in_num_channels, in_input_channels)
{
    state = silent;
    triggerTime = 0;

    attack_time  = in_attack_time  * Portaudio::DEFAULT_SAMPLE_RATE;
    decay_time   = in_decay_time   * Portaudio::DEFAULT_SAMPLE_RATE;
    release_time = in_release_time * Portaudio::DEFAULT_SAMPLE_RATE;

    sustain_level = in_sustain_level;
}


void ADSRFilter::on_note_down()
{
    state = attack;
    triggerTime = t;
}


void ADSRFilter::on_note_up()
{
    state = release;
    triggerTime = t;
}


void ADSRFilter::filter(SAMPLE** input, SAMPLE** output)
{
    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
    {
        // Update the state if nessecary, compute multiplier
        t = next_t+i;
        float multiplier;
        switch(state)
        {
            case silent:
                multiplier = 0.0;
                break;

            case attack:
                if(t >= triggerTime + attack_time)
                {
                    state = decay;
                    triggerTime = t;
                    multiplier = 1.0;
                }
                else
                {
                    multiplier = ((float)t-triggerTime)/attack_time;
                }
                break;

            case decay:
                if(t >= triggerTime + decay_time)
                {
                    state = sustain;
                    triggerTime = t;
                    multiplier = sustain_level;
                }
                else
                {
                    float time_p = ((float)t-triggerTime)/attack_time;
                    multiplier = 1.0 - (1.0-sustain_level)*time_p;
                }
                break;

            case sustain:
                multiplier = sustain_level;
                break;

            case release:
                if(t >= triggerTime + release_time)
                {
                    state = silent;
                    triggerTime = t;
                    multiplier = 0.0;
                }
                else
                {
                    float time_p = ((float)t-triggerTime)/attack_time;
                    multiplier = sustain_level - sustain_level*time_p;
                }
                break;
        }
        
        for(int j = 0; j < num_input_channels; j++)
        {
            output[j][i] = multiplier * input[j][i];
        }
    }
}
