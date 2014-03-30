#include "multiplier.h"

using namespace ClickTrack;


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
