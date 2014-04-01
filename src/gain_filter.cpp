#include <cmath>
#include "gain_filter.h"

using namespace ClickTrack;


GainFilter::GainFilter(float in_gain, unsigned num_channels)
    : AudioFilter(num_channels, num_channels), gain(pow(10, in_gain/10)),
      lfo(nullptr), lfo_intensity(0.0)
{}

void GainFilter::set_gain(float in_gain)
{
    gain = pow(10, in_gain/10);
}

void GainFilter::set_lfo_input(Channel* input)
{
    lfo = input;
}

void GainFilter::set_lfo_intensity(float db)
{
    lfo_intensity = db;
}

void GainFilter::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t)
{
    for(int i = 0; i < input.size(); i++)
    {
        // Use the LFO if set
        float m = 1.0;
        if(lfo != nullptr)
            m *= pow(10, lfo->get_sample(t) * lfo_intensity/10);

        output[i] = gain*m*input[i];
    }
}
