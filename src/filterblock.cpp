#include <iostream>
#include "filterblock.h"

using namespace std;
using namespace FilterChain;


AudioElement::AudioElement(unsigned num_channels)
    : out_buffers(num_channels,
      ClickTrackUtils::RingBuffer<SAMPLE>(DEFAULT_BUFFER_SIZE))
{
    num_output_channels = num_channels;
}


SAMPLE* AudioElement::get_sample_block(unsigned output_channel, unsigned block_id)
{
    while(next_block < block_id)
    {
        generate_output();
        next_block++;
    }

    SAMPLE* block = new SAMPLE[DEFAULT_BLOCK_SIZE];

    for(unsigned t = 0; t < DEFAULT_BLOCK_SIZE; t++)
    {
        block[t] = out_buffers[output_channel].
            get_sample(block_id*DEFAULT_BLOCK_SIZE + t);
    }

    return block;
}


FilterElement::FilterElement(AudioElement* input_elements,
                           unsigned* input_chans,
                           unsigned num_input_elements,
                           unsigned num_channels)
    : AudioElement(num_channels)
{
    num_inputs = num_input_elements;
    inputs = input_elements;
    input_channels = input_chans;
}


void FilterElement::generate_output(unsigned block_id)
{
    SAMPLE** input_samples = new SAMPLE*[num_inputs]; 

    for(int i = 0; i < num_inputs; i++)
        input_samples[i] = inputs[i].get_sample_block(input_channels[i], block_id);

    run_filter(input_samples);
}


OutputElement::OutputElement(AudioElement* input_elements, unsigned* input_chans,
                             unsigned num_input_elements)
{
    num_inputs = num_input_elements;
    input_channels = input_chans;
    inputs = input_elements;

    next_block = 0;
}


void OutputElement::process_inputs()
{
    SAMPLE** input_samples = new SAMPLE*[num_inputs]; 

    for(int i = 0; i < num_inputs; i++)
        input_samples[i] = inputs[i].get_sample_block(input_channels[i], next_block);

    next_block++;
    output(input_samples);
}
