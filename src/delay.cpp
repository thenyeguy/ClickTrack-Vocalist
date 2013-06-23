#include <iostream>
#include "delay.h"

using namespace FilterGenerics;
using namespace Filters;

//std::vector<ClickTrackUtils::RingBuffer<SAMPLE> > last_second;

SimpleDelay::SimpleDelay(OutputChannel** in_input_channels,
                         unsigned in_num_channels)
    : AudioFilter(in_num_channels, in_num_channels,
                  in_input_channels),
      last_second(num_input_channels, ClickTrackUtils::RingBuffer<SAMPLE>(44100))
{}

void SimpleDelay::filter(SAMPLE** input, SAMPLE** output)
{
    // Calculate offset
    unsigned last_t = next_t - 44100;

    // Sum in the output
    for(int i = 0; i < num_input_channels; i++)
    {
        for(int j = 0; j < DEFAULT_BLOCK_SIZE; j++)
        {
            output[i][j] = input[i][j];
            if(next_t > DEFAULT_BLOCK_SIZE+44100)
                output[i][j] += last_second[i].get(last_t+j)/2;
        }
    }

    // Write inputs into tracking buffers
    for(int i = 0; i < num_input_channels; i++)
    {
        for(int j = 0; j < DEFAULT_BLOCK_SIZE; j++)
            last_second[i].add(input[i][j]);
    }
}
