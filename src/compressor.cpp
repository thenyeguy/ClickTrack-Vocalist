#include <cmath>
#include "compressor.h"

using namespace ClickTrack;


Compressor::Compressor(float in_threshold, float in_compression_ratio,
        float in_gain, float in_lookahead)
    : AudioFilter(1,1), 
      lookahead(SAMPLE_RATE*in_lookahead),
      threshold(pow(10, in_threshold/20)),
      compression_ratio(in_compression_ratio),
      gain(pow(10, in_gain/20)),
      envelope(), 
      inputs(lookahead+1)
{}


void Compressor::set_gain(float in_gain)
{
    gain = pow(10, in_gain/20);
}


void Compressor::set_compression_ratio(float in_compression_ratio)
{
    compression_ratio = in_compression_ratio;
}


void Compressor::set_threshold(float in_threshold)
{
    threshold = pow(10, in_threshold/20);
}


void Compressor::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t)
{
    // Push the sample using the lookahead
    inputs.add(input[0]);
    float in = 0.0;
    if(t >= lookahead)
        in = inputs[t-lookahead];

    // Calculate the limiter gain from the threshold
    float compression = 1.0;
    float level = envelope.get_next_level(input[0]);
    if(level > threshold)
        compression = compression_ratio*threshold/level + 1-compression_ratio;

    // Set the output with the limiter_gain
    output[0] = gain*compression*in;
}
