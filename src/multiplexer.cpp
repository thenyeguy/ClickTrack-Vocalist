#include "multiplexer.h"

using namespace ClickTrack;


Multiplexer::Multiplexer(unsigned in_num_input_channels)
    : AudioFilter(in_num_input_channels, 1), channel(0)
{}

void Multiplexer::select_channel(unsigned in_channel)
{
    channel = in_channel;
}

void Multiplexer::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t)
{
    output[0] = input[channel];
}
