#include "gain_filter.h"

using namespace ClickTrack;


GainFilter::GainFilter(float in_gain, unsigned num_channels)
    : AudioFilter(num_channels, num_channels), gain(in_gain)
{}

void GainFilter::set_gain(float in_gain)
{
    gain = in_gain;
}

void GainFilter::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t)
{
    for(int i = 0; i < input.size(); i++)
    {
        output[i] = gain*input[i];
    }
}
