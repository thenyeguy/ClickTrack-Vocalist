#include <cmath>
#include "reverb.h"
#include "io_elements.h"

using namespace ClickTrack;


SimpleReverb::SimpleReverb(float in_decay_time, float in_gain, float in_wetness,
                           unsigned in_num_channels)
    : AudioFilter(in_num_channels, in_num_channels),
      gain(in_gain), wetness(in_wetness)
{}
      

void SimpleReverb::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t)
{
    (void) gain;
    (void) wetness;
    // TODO: fill
}




/*
 * ConvolutionReverb::ConvolutionReverb(unsigned impulse_length, SAMPLE* impulse,
 *                float in_gain, float in_wetness)
 *     : FilterBank(1,1), gain(in_gain), wetness(in_wetness),
 *       conv(impulse_length, impulse), wet(wetness*gain), dry((1.0-wetness)*gain),
 *       out(2)
 * {
 *     // Connect our signal chain
 *     wet.set_input_channel(conv.get_output_channel());
 *     out.set_input_channel(wet.get_output_channel(), 0);
 *     out.set_input_channel(dry.get_output_channel(), 1);
 * 
 *     output_channels.push_back(out.get_output_channel());
 * }
 * 
 * 
 * void ConvolutionReverb::set_input_channel(Channel* in_input_channel,
 *         unsigned channel_i)
 * {
 *     conv.set_input_channel(in_input_channel);
 *     dry.set_input_channel(in_input_channel);
 * }
 * 
 * 
 * void ConvolutionReverb::remove_channel(unsigned channel_i)
 * {
 *     conv.remove_channel(channel_i);
 * }
 * 
 * 
 * unsigned ConvolutionReverb::get_channel_index(Channel* channel)
 * {
 *     return conv.get_channel_index(channel);
 * }
 */




impulse_pair* ClickTrack::impulse_from_wav(const char* filename)
{
    WavReader wav(filename);

    impulse_pair* out = new impulse_pair;
    out->num_samples = wav.get_total_samples();
    out->left = new SAMPLE[out->num_samples];
    out->right = new SAMPLE[out->num_samples];

    for(unsigned t = 0; t < out->num_samples; t++)
    {
        out->left[t] = wav.get_output_channel(0)->get_sample(t);
        out->right[t] = wav.get_output_channel(0)->get_sample(t);
    }

    return out;
}
