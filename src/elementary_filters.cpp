#include <iostream>
#include "elementary_filters.h"

using namespace FilterGenerics;
using namespace Filters;


UnityFilter::UnityFilter(OutputChannel** in_input_channels, 
                         unsigned in_num_channels)
    : AudioFilter(in_num_channels, in_num_channels,
                  in_input_channels)
{}

void UnityFilter::filter(SAMPLE** input, SAMPLE** output)
{
    // Sum in the output
    for(int i = 0; i < num_input_channels; i++)
    {
        for(int j = 0; j < DEFAULT_BLOCK_SIZE; j++)
        {
            output[i][j] = input[i][j];
        }
    }
}




GainFilter::GainFilter(float in_gain,
                       OutputChannel** in_input_channels, 
                       unsigned in_num_channels)
    : AudioFilter(in_num_channels, in_num_channels,
                  in_input_channels)
{
    gain = in_gain;    
}

void GainFilter::set_gain(float in_gain)
{
    gain = in_gain;
}

void GainFilter::filter(SAMPLE** input, SAMPLE** output)
{
    // Sum in the output
    for(int i = 0; i < num_input_channels; i++)
    {
        for(int j = 0; j < DEFAULT_BLOCK_SIZE; j++)
        {
            output[i][j] = gain*input[i][j];
        }
    }
}




Adder::Adder(OutputChannel** in_input_channels, unsigned in_num_channels)
    : AudioFilter(1, in_num_channels, in_input_channels)
{}

void Adder::filter(SAMPLE** input, SAMPLE** output)
{
    // Sum in the output
    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
    {
        output[0][i] = 0;
        for(int j = 0; j < num_input_channels; j++)
        {
            output[0][i] += input[j][i];
        }
    }
}




Multiplier::Multiplier(OutputChannel** in_input_channels, unsigned in_num_channels)
    : AudioFilter(1, in_num_channels, in_input_channels)
{}

void Multiplier::filter(SAMPLE** input, SAMPLE** output)
{
    // Sum in the output
    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
    {
        output[0][i] = 0;
        for(int j = 0; j < num_input_channels; j++)
        {
            output[0][i] *= input[j][i];
        }
    }
}





ClipDetector::ClipDetector(float in_rate, OutputChannel** in_input_channels, 
                           unsigned in_num_channels)
    : AudioFilter(in_num_channels, in_num_channels,
                  in_input_channels), rate(in_rate*44100), next_time(0)
{}

void ClipDetector::filter(SAMPLE** input, SAMPLE** output)
{
    // Sum in the output
    for(int i = 0; i < num_input_channels; i++)
    {
        for(int j = 0; j < DEFAULT_BLOCK_SIZE; j++)
        {
            if(abs(input[i][j]) > 1.0 && next_t+j > next_time)
            {
                std::cout << "AUDIO CLIPPING DETECTED" << std::endl;
                next_time = next_t+j + rate;
            }

            output[i][j] = input[i][j];
        }
    }
}
