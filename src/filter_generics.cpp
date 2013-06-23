#include "filter_generics.h"


using namespace FilterGenerics;


OutputChannel::OutputChannel(AudioGenerator* in_parent)
    : out(DEFAULT_BUFFER_SIZE)
{
    parent = in_parent;

    start_t = 0;
    end_t = 0;
}


void OutputChannel::get_block(SAMPLE* buffer, const unsigned t)
{
    // If this block already fell out of the buffer, just return silence
    if(start_t >= t+DEFAULT_BLOCK_SIZE)
    {
        for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
            buffer[i] = 0.0;
        return;
    }

    // Otherwise generate enough audio
    while(end_t < t+DEFAULT_BLOCK_SIZE)
        parent->write_outputs();

    out.get_range(buffer, start_t, start_t+DEFAULT_BLOCK_SIZE);
}


void OutputChannel::push_block(const SAMPLE* buffer)
{
    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
        out.add(buffer[i]);

    start_t = out.get_lowest_timestamp();
    end_t = out.get_highest_timestamp();
}



AudioGenerator::AudioGenerator(const unsigned in_num_output_channels)
    : num_output_channels(in_num_output_channels),
      output_channels(in_num_output_channels, OutputChannel(this))
{
    // Allocate the buffer once for writing to
    output_buffer = new SAMPLE*[num_output_channels];
    for(int i = 0; i < num_output_channels; i++)
        output_buffer[i] = new SAMPLE[DEFAULT_BLOCK_SIZE];
}


AudioGenerator::~AudioGenerator()
{
    for(int i = 0; i < num_output_channels; i++)
        delete output_buffer[i];
    delete output_buffer;
}


OutputChannel* AudioGenerator::get_output_channel(int i)
{
    return &output_channels[i];
}


void AudioGenerator::write_outputs()
{
    generate_outputs(output_buffer);

    //Write the outputs into the channel
    for(int i = 0; i < num_output_channels; i++)
        output_channels[i].push_block(output_buffer[i]);
}




AudioConsumer::AudioConsumer(unsigned in_num_input_channels,
        OutputChannel** in_input_channels)
    : num_input_channels(in_num_input_channels)
{
    next_t = 0;
    
    for(int i = 0; i < num_input_channels; i++)
        input_channels.push_back(in_input_channels[i]);

    // Allocate the buffer once for reading in
    input_buffer = new SAMPLE*[num_input_channels];
    for(int i = 0; i < num_input_channels; i++)
        input_buffer[i] = new SAMPLE[DEFAULT_BLOCK_SIZE];
}


AudioConsumer::~AudioConsumer()
{
    for(int i = 0; i < num_input_channels; i++)
        delete input_buffer[i];
    delete input_buffer;
}


void AudioConsumer::consume_inputs()
{
    // Read in each channel
    for(int i = 0; i < num_input_channels; i++)
        input_channels[i]->get_block(input_buffer[i], next_t);

    // Process
    process_inputs(input_buffer);
    next_t += DEFAULT_BLOCK_SIZE;
}




AudioFilter::AudioFilter(unsigned in_num_output_channels,
                         unsigned in_num_input_channels,
                         OutputChannel** in_input_channels)
    : AudioGenerator(in_num_output_channels),
      AudioConsumer(in_num_input_channels, in_input_channels) {}


void AudioFilter::generate_outputs(SAMPLE** outputs)
{
    consume_inputs();
}


void AudioFilter::process_inputs(SAMPLE** inputs)
{
    filter(inputs, output_buffer);
}
