#include <cmath>
#include "noise_gate.h"

using namespace ClickTrack;


NoiseGate::NoiseGate(float in_on_threshold, float in_off_threshold, 
        float in_gain, float in_lookahead)
    : AudioFilter(1,1), 
      lookahead(SAMPLE_RATE*in_lookahead),
      on_threshold(pow(10, in_on_threshold/20)),
      off_threshold(pow(10, in_off_threshold/20)),
      gain(pow(10, in_gain/20)),
      active(false),
      envelope(), 
      inputs(lookahead+1)
{}


void NoiseGate::set_gain(float in_gain)
{
    gain = pow(10, in_gain/20);
}


void NoiseGate::set_on_threshold(float in_threshold)
{
    on_threshold = pow(10, in_threshold/20);
}


void NoiseGate::set_off_threshold(float in_threshold)
{
    off_threshold = pow(10, in_threshold/20);
}


void NoiseGate::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t)
{
    // Push the sample using the lookahead
    inputs.add(input[0]);
    float in = 0.0;
    if(t >= lookahead)
        in = inputs[t-lookahead];

    // Calculate the limiter gain from the threshold
    float level = envelope.get_next_level(input[0]);
    if(active && level < off_threshold)
        active = false;
    if(!active && level > on_threshold)
        active = true;

    if(active) output[0] = in;
    else       output[0] = 0.0;
}
