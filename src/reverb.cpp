#include <cmath>
#include "reverb.h"
#include "io_elements.h"

using namespace ClickTrack;


SimpleReverb::SimpleReverb(float in_decay_time, float in_gain, float in_wetness,
                           unsigned in_num_channels)
    : AudioFilter(in_num_channels, in_num_channels),
      sample_rollover(in_num_channels, 0.0), gain(in_gain), wetness(in_wetness),
      decay_ratio(pow(.01, 1/(SAMPLE_RATE*in_decay_time)))
{}


SimpleReverb::~SimpleReverb()
{}
      

void SimpleReverb::filter(std::vector< std::vector<SAMPLE> >& input,
        std::vector< std::vector<SAMPLE> >& output)
{
    for(int i = 0; i < num_input_channels; i++)
    {
        for(int j = 0; j < FRAME_SIZE; j++)
        {
            SAMPLE last;
            if(j == 0) last = sample_rollover[i];
            else       last = input[i][j-1];

            output[i][j] = gain*(input[i][j] + wetness*decay_ratio*last);
        }
    }
}




ConvolutionReverb::ConvolutionReverb(unsigned impulse_length, SAMPLE* impulse,
               float in_gain, float in_wetness)
    : FilterBank(1,1), gain(in_gain), wetness(in_wetness),
      conv(impulse_length, impulse), wet(wetness*gain), dry((1.0-wetness)*gain),
      out(2)
{
    // Connect our signal chain
    wet.set_input_channel(conv.get_output_channel());
    out.set_input_channel(wet.get_output_channel(), 0);
    out.set_input_channel(dry.get_output_channel(), 1);

    output_channels.push_back(out.get_output_channel());
}


void ConvolutionReverb::set_input_channel(Channel* in_input_channel,
        unsigned channel_i)
{
    conv.set_input_channel(in_input_channel);
    dry.set_input_channel(in_input_channel);
}


void ConvolutionReverb::remove_channel(unsigned channel_i)
{
    conv.remove_channel(channel_i);
}


unsigned ConvolutionReverb::get_channel_index(Channel* channel)
{
    return conv.get_channel_index(channel);
}




impulse_pair* ClickTrack::impulse_from_wav(const char* filename)
{
    WavReader wav(filename);

    impulse_pair* out = new impulse_pair;
    out->num_samples = wav.get_total_samples();
    out->left = new SAMPLE[out->num_samples];
    out->right = new SAMPLE[out->num_samples];

    std::vector<SAMPLE> buffer(FRAME_SIZE);
    for(unsigned t = 0; t < out->num_samples; t += FRAME_SIZE)
    {
        // Read left and copy in
        wav.get_output_channel(0)->get_frame(buffer, t);
        for(unsigned i = 0; i < FRAME_SIZE; i++)
            out->left[t+i] = buffer[i];

        // Read right and copy in
        wav.get_output_channel(1)->get_frame(buffer, t);
        for(unsigned i = 0; i < FRAME_SIZE; i++)
            out->right[t+i] = buffer[i];
    }

    return out;
}
