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
        parent->write_outputs();

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

    // Allocate the buffer once for writing to
    output_buffer = new SAMPLE*[num_output_channels];
    for(int i = 0; i < num_output_channels; i++)
        output_buffer[i] = new SAMPLE[DEFAULT_BLOCK_SIZE];
}


OutputChannel& AudioGenerator::get_output_channel(int i)
{
    return output_channels[i];
}


void AudioGenerator::write_outputs()
{
    generate_outputs(output_buffer);

    //Write the outputs into the channel
    for(int i = 0; i < num_output_channels; i++)
        output_channels[i].fill_internal_buffer(output_buffer[i]);
}




AudioConsumer::AudioConsumer(const unsigned in_num_input_channels,
        OutputChannel* in_input_channels)
{
    next_t = 0;
    
    num_input_channels = in_num_input_channels;
    for(int i = 0; i < num_input_channels; i++)
        input_channels.push_back(&in_input_channels[i]);

    // Allocate the buffer once for reading in
    input_buffer = new SAMPLE*[num_input_channels];
    for(int i = 0; i < num_input_channels; i++)
        input_buffer[i] = new SAMPLE[DEFAULT_BLOCK_SIZE];
}


void AudioConsumer::consume_inputs()
{

    // Read in each channel
    for(int i = 0; i < num_input_channels; i++)
        input_channels[i]->fill_external_buffer(input_buffer[i], next_t);
    next_t += DEFAULT_BLOCK_SIZE;

    // Process
    process_inputs(input_buffer);
}




AudioFilter::AudioFilter(const unsigned in_num_output_channels,
        const unsigned in_num_input_channels,
        OutputChannel* in_input_channels)
    : AudioGenerator(in_num_output_channels),
      AudioConsumer(in_num_input_channels, in_input_channels) {}


void AudioFilter::generate_outputs(SAMPLE** outputs)
{
    consume_inputs();
}


void AudioFilter::process_inputs(SAMPLE** inputs)
{
    filter(inputs, output_buffer);

    // Copy and delete data
    for(int i = 0; i < num_output_channels; i++)
        output_channels[i].fill_internal_buffer(output_buffer[i]);
}
