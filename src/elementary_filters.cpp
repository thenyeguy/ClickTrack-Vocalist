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
