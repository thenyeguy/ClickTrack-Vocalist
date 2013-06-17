#include "filtergenerics.h"


using namespace FilterGenerics;


OutputChannel::OutputChannel(unsigned in_channel_id,
                             AudioGenerator* in_parent)
    : out(DEFAULT_BUFFER_SIZE)
{
    channel_id = in_channel_id;
    parent = in_parent;

    start_t = 0;
    end_t = 0;
}


void OutputChannel::fill_external_buffer(SAMPLE* buffer, const unsigned t)
{
    while(start_t < t || end_t < t+DEFAULT_BLOCK_SIZE)
        parent->generate_outputs();

    out.get_range(buffer, start_t, start_t+DEFAULT_BLOCK_SIZE);
}


void OutputChannel::fill_internal_buffer(const SAMPLE* buffer)
{
    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
        out.add(buffer[i]);

    start_t = out.get_lowest_timestamp();
    end_t = out.get_highest_timestamp();
}



AudioGenerator::AudioGenerator(const unsigned in_num_output_channels)
    : num_output_channels(in_num_output_channels)
{
    for(int i = 0; i < in_num_output_channels; i++)
        output_channels.push_back(OutputChannel(i, this));
}


OutputChannel& AudioGenerator::get_output_channel(int i)
{
    return output_channels[i];
}




AudioConsumer::AudioConsumer(const unsigned in_num_input_channels,
        OutputChannel* in_input_channels)
{
    next_t = 0;
    
    num_input_channels = in_num_input_channels;
    for(int i = 0; i < num_input_channels; i++)
        input_channels.push_back(&in_input_channels[i]);
}


void AudioConsumer::consume_inputs()
{
    SAMPLE** inputs = new SAMPLE*[num_input_channels];

    // Read in each channel
    for(int i = 0; i < num_input_channels; i++)
    {
        inputs[i] = new SAMPLE[DEFAULT_BLOCK_SIZE];
        input_channels[i]->fill_external_buffer(inputs[i], next_t);
    }
    next_t += DEFAULT_BLOCK_SIZE;

    // Process then delete inputs
    process_inputs(inputs);
    for(int i = 0; i < num_input_channels; i++)
        delete inputs[i];
    delete inputs;
}




AudioFilter::AudioFilter(const unsigned in_num_output_channels,
        const unsigned in_num_input_channels,
        OutputChannel* in_input_channels)
    : AudioGenerator(in_num_output_channels),
      AudioConsumer(in_num_input_channels, in_input_channels) {}


void AudioFilter::generate_outputs()
{
    consume_inputs();
}


void AudioFilter::process_inputs(SAMPLE** inputs)
{
    SAMPLE** outputs = new SAMPLE*[num_output_channels];
    for(int i = 0; i < num_output_channels; i++)
        outputs[i] = new SAMPLE[DEFAULT_BLOCK_SIZE];

    filter(inputs, outputs);

    // Copy and delete data
    for(int i = 0; i < num_output_channels; i++)
    {
        output_channels[i].fill_internal_buffer(outputs[i]);
        delete outputs[i];
    }
    delete outputs;
}
