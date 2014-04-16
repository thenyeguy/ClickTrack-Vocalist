#include <cmath>
#include <iostream>
#include "limiter.h"

using namespace ClickTrack;


Limiter::Limiter(float in_threshold, float in_lookahead)
    : AudioFilter(1,1), 
      lookahead(SAMPLE_RATE*in_lookahead),
      threshold(pow(10, in_threshold/20)),
      envelope(), 
      inputs(lookahead)
{}


void Limiter::set_threshold(float in_threshold)
{
    threshold = pow(10, in_threshold/20);
}


void Limiter::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t)
{
    // Calculate the gain from the threshold
    float gain = 1.0;
    float level = envelope.get_next_level(input[0]);
    if(level > threshold)
    {
        std::cout << t << ": " << level << std::endl;
        gain = threshold / level;
    }

    // Set the output with the gain
    output[0] = gain*input[0];
}
