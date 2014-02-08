#include <cmath>
#include "reverb.h"
#include "io_elements.h"

using namespace Filters;
using namespace Portaudio;


SimpleReverb::SimpleReverb(float in_decay_time, float in_gain, float in_wetness,
                           unsigned in_num_channels)
    : AudioFilter(in_num_channels, in_num_channels),
      sample_rollover(in_num_channels, 0.0), gain(in_gain), wetness(in_wetness),
      decay_ratio(pow(.01, 1/(DEFAULT_SAMPLE_RATE*in_decay_time)))
{}


SimpleReverb::~SimpleReverb()
{}
      

void SimpleReverb::filter(std::vector< std::vector<SAMPLE> >& input,
        std::vector< std::vector<SAMPLE> >& output)
{
    for(int i = 0; i < num_input_channels; i++)
    {
        for(int j = 0; j < DEFAULT_BLOCK_SIZE; j++)
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
    : gain(in_gain), wetness(in_wetness), conv(impulse_length, impulse),
      wet(wetness*gain), dry((1.0-wetness)*gain), out(2)
{
    // Connect our signal chain
    wet.set_input_channel(conv.get_output_channel());
    out.set_input_channel(wet.get_output_channel(), 0);
    out.set_input_channel(dry.get_output_channel(), 1);
}


void ConvolutionReverb::set_input_channel(Channel* in_input_channel)
{
    dry.set_input_channel(in_input_channel);
}


Channel* ConvolutionReverb::get_output_channel()
{
    return out.get_output_channel();
}




impulse_pair* Filters::impulse_from_wav(const char* filename)
{
    IOElements::WavReader wav(filename);

    impulse_pair* out = new impulse_pair;
    out->num_samples = wav.get_total_samples();
    out->left = new SAMPLE[out->num_samples];
    out->right = new SAMPLE[out->num_samples];

    std::vector<SAMPLE> buffer(DEFAULT_BLOCK_SIZE);
    for(unsigned t = 0; t < out->num_samples; t += FilterGenerics::DEFAULT_BLOCK_SIZE)
    {
        // Read left and copy in
        wav.get_output_channel(0)->get_block(buffer, t);
        for(unsigned i = 0; i < DEFAULT_BLOCK_SIZE; i++)
            out->left[t+i] = buffer[i];

        // Read right and copy in
        wav.get_output_channel(1)->get_block(buffer, t);
        for(unsigned i = 0; i < DEFAULT_BLOCK_SIZE; i++)
            out->right[t+i] = buffer[i];
    }

    return out;
}
