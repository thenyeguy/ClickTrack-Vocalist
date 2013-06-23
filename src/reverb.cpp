#include "reverb.h"
#include "io_elements.h"

using namespace Filters;


Reverb::Reverb(unsigned impulse_length, SAMPLE* impulse, float in_wetness,
               OutputChannel* in_input_channel)
    : FilterBank(1,1), wetness(in_wetness)
{
    // Set up reverb
    conv = new ConvolutionFilter(in_input_channel, impulse_length, impulse);
    OutputChannel* conv_out = conv->get_output_channel();

    // Set up wetness parameters
    wet = new GainFilter(wetness, &conv_out);
    dry = new GainFilter(1.0-wetness, &in_input_channel);

    // Set up output adder
    OutputChannel* wetdry[2] =
        {wet->get_output_channel(), dry->get_output_channel()};
    out = new Adder(wetdry, 2);

    // Add the output channel
    output_channels.push_back(out->get_output_channel());
    //output_channels.push_back(out->get_output_channel());
}


Reverb::~Reverb()
{
    delete conv;
    delete wet;
    delete dry;
    delete out;
}


impulse_pair* Filters::impulse_from_wav(const char* filename)
{
    IOElements::WavReader wav(filename);

    impulse_pair* out = new impulse_pair;
    out->num_samples = wav.get_total_samples();
    out->left = new SAMPLE[out->num_samples];
    out->right = new SAMPLE[out->num_samples];

    for(unsigned t = 0; t < out->num_samples; t += FilterGenerics::DEFAULT_BLOCK_SIZE)
    {
        wav.get_output_channel(0)->get_block(&out->left[t], t);
        wav.get_output_channel(1)->get_block(&out->right[t], t);
    }

    return out;
}
