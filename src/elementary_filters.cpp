#include <iostream>
#include "elementary_filters.h"

using namespace FilterGenerics;
using namespace Filters;


GainFilter::GainFilter(float in_gain, unsigned num_channels)
    : AudioFilter(num_channels, num_channels), gain(in_gain)
{}

void GainFilter::set_gain(float in_gain)
{
    gain = in_gain;
}

void GainFilter::filter(std::vector< std::vector<SAMPLE> >& input,
        std::vector< std::vector<SAMPLE> >& output)
{
    // Sum in the output
    for(int i = 0; i < num_input_channels; i++)
    {
        for(int j = 0; j < BLOCK_SIZE; j++)
        {
            output[i][j] = gain*input[i][j];
        }
    }
}




Adder::Adder(unsigned in_num_input_channels)
    : AudioFilter(in_num_input_channels, 1)
{}

void Adder::filter(std::vector< std::vector<SAMPLE> >& input,
        std::vector< std::vector<SAMPLE> >& output)
{
    // Sum in the output
    for(int i = 0; i < BLOCK_SIZE; i++)
    {
        output[0][i] = 0;
        for(int j = 0; j < num_input_channels; j++)
        {
            output[0][i] += input[j][i];
        }
    }
}




Multiplier::Multiplier(unsigned in_num_input_channels)
    : AudioFilter(in_num_input_channels, 1)
{}

void Multiplier::filter(std::vector< std::vector<SAMPLE> >& input,
        std::vector< std::vector<SAMPLE> >& output)
{
    // Sum in the output
    for(int i = 0; i < BLOCK_SIZE; i++)
    {
        output[0][i] = 0;
        for(int j = 0; j < num_input_channels; j++)
        {
            output[0][i] *= input[j][i];
        }
    }
}





ClipDetector::ClipDetector(float in_rate, unsigned num_channels)
    : AudioFilter(num_channels), rate(in_rate*44100), next_time(0)
{}

void ClipDetector::filter(std::vector< std::vector<SAMPLE> >& input,
        std::vector< std::vector<SAMPLE> >& output)
{
    // Sum in the output
    for(int i = 0; i < num_input_channels; i++)
    {
        for(int j = 0; j < BLOCK_SIZE; j++)
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
