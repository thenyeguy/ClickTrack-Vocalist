#include <iostream>
#include "basic_elements.h"

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




Adder::Adder(unsigned in_num_input_channels)
    : AudioFilter(in_num_input_channels, 1)
{}

void Adder::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t)
{
    output[0] = 0;
    for(int i = 0; i < input.size(); i++)
    {
        output[0] += input[i];
    }
}




Multiplier::Multiplier(unsigned in_num_input_channels)
    : AudioFilter(in_num_input_channels, 1)
{}

void Multiplier::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t)
{
    // Sum in the output
    output[0] = 0;
    for(int i = 0; i < input.size(); i++)
    {
        output[0] *= input[i];
    }
}





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





ClipDetector::ClipDetector(float in_rate, unsigned num_channels)
    : AudioFilter(num_channels), rate(in_rate*44100), next_time(0)
{}

void ClipDetector::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t)
{
    for(int i = 0; i < input.size(); i++)
    {
        if(abs(input[i]) >= 1.0 && t > next_time)
        {
            std::cout << "AUDIO CLIPPING DETECTED" << std::endl;
            next_time = t + rate;
        }

        // Copy inputs to outpts
        output[i] = input[i];
    }
}
