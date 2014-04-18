#include <cmath>
#include "limiter.h"

using namespace ClickTrack;


Limiter::Limiter(float in_threshold, float in_gain, float in_lookahead)
    : AudioFilter(1,1), 
      lookahead(SAMPLE_RATE*in_lookahead),
      threshold(pow(10, in_threshold/20)),
      gain(pow(10, in_gain/20)),
      envelope(), 
      inputs(lookahead+1)
{}


void Limiter::set_gain(float in_gain)
{
    gain = pow(10, in_gain/20);
}


void Limiter::set_threshold(float in_threshold)
{
    threshold = pow(10, in_threshold/20);
}


void Limiter::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t)
{
    // Push the sample using the lookahead
    inputs.add(input[0]);
    float in = 0.0;
    if(t >= lookahead)
        in = inputs[t-lookahead];

    // Calculate the limiter gain from the threshold
    float limiter_gain = 1.0;
    float level = envelope.get_next_level(input[0]);
    if(level > threshold)
        limiter_gain = threshold / level;

    // Set the output with the limiter_gain
    output[0] = gain*limiter_gain*in;
}
