#include <iostream>
#include "adsr.h"

using namespace std;
using namespace FilterGenerics;
using namespace Filters;


ADSRFilter::ADSRFilter(float in_attack_time, float in_decay_time,
                       float in_sustain_level, float in_release_time,
                       unsigned in_num_channels)
    : AudioFilter(in_num_channels, in_num_channels), state(silent),
    t(0), trigger_time(0), end_time(0), multiplier(0), delta_mult(0)
{
    attack_time  = in_attack_time  * Portaudio::DEFAULT_SAMPLE_RATE;
    decay_time   = in_decay_time   * Portaudio::DEFAULT_SAMPLE_RATE;
    release_time = in_release_time * Portaudio::DEFAULT_SAMPLE_RATE;

    sustain_level = in_sustain_level;
}


void ADSRFilter::on_note_down()
{
    state = attack;
    trigger_time = next_t;
    end_time = next_t + attack_time;
    
    delta_mult = (1.0 - multiplier)/attack_time;
}


void ADSRFilter::on_note_up()
{
    state = release;
    trigger_time = next_t;
    end_time = next_t + release_time;

    delta_mult = (0.0 - multiplier)/release_time;
}


void ADSRFilter::filter(std::vector< std::vector<SAMPLE> >& input,
        std::vector< std::vector<SAMPLE> >& output)
{
    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
    {
        // Update the time and multiplier
        t = next_t+i;
        multiplier += delta_mult;

        // Update the state if nessecary
        if(t >= end_time)
        {
            switch(state)
            {
                case attack:
                    state = decay;
                    trigger_time = t;
                    end_time = t + decay_time;

                    multiplier = 1.0;
                    delta_mult = (sustain_level - 1.0)/decay_time;
                    break;

                case decay:
                    state = sustain;
                    trigger_time = t;
                    end_time = t;

                    multiplier = sustain_level;
                    delta_mult = 0.0;
                    break;

                case release:
                    state = silent;
                    trigger_time = t;
                    end_time = t;

                    multiplier = 0.0;
                    delta_mult = 0.0;
                    break;
                
                default:
                    // Do nothing, no other states end themselves
                    break;
            }
        }


        // Copy the new set of samples to our output
        for(int j = 0; j < num_input_channels; j++)
        {
            output[j][i] = multiplier * input[j][i];
        }
    }
}
