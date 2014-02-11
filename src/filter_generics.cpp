#include <iostream>
#include "filter_generics.h"


using namespace FilterGenerics;


Channel::Channel(AudioGenerator* in_parent, unsigned in_start_t)
    : out(DEFAULT_RINGBUFFER_SIZE)
{
    parent = in_parent;

    start_t = in_start_t;
    end_t = in_start_t;

    out.set_new_startpoint(start_t);
}


void Channel::get_block(std::vector<SAMPLE>& buffer, const unsigned t)
{
    // If this block already fell out of the buffer, just return silence
    if(start_t >= t+BLOCK_SIZE)
    {
        std::cerr << "Channel has requested a time older than is in "
            << "its buffer." << std::endl;
        for(int i = 0; i < BLOCK_SIZE; i++)
            buffer[i] = 0.0;
        return;
    }

    // Otherwise generate enough audio
    while(end_t < t+BLOCK_SIZE)
        parent->write_outputs();

    out.get_range(buffer, start_t, start_t+BLOCK_SIZE);
}


void Channel::push_block(const std::vector<SAMPLE>& buffer)
{
    for(int i = 0; i < BLOCK_SIZE; i++)
        out.add(buffer[i]);

    start_t = out.get_lowest_timestamp();
    end_t = out.get_highest_timestamp();
}




AudioGenerator::AudioGenerator(unsigned in_num_output_channels)
    : next_out_t(0), num_output_channels(in_num_output_channels),
      output_channels(), output_buffer()
{
    for(unsigned i = 0; i < in_num_output_channels; i++)
    {
        output_channels.push_back(Channel(this));
        output_buffer.push_back(std::vector<SAMPLE>(BLOCK_SIZE));
    }
}


Channel* AudioGenerator::get_output_channel(unsigned i)
{
    if(i >= num_output_channels)
        throw ChannelOutOfRange();
    return &output_channels[i];
}


unsigned AudioGenerator::get_num_output_channels()
{
    return num_output_channels;
}


void AudioGenerator::write_outputs()
{
    generate_outputs(output_buffer);
    next_out_t += BLOCK_SIZE;

    //Write the outputs into the channel
    for(int i = 0; i < num_output_channels; i++)
        output_channels[i].push_block(output_buffer[i]);
}




AudioConsumer::AudioConsumer(unsigned in_num_input_channels)
    : next_t(0), num_input_channels(in_num_input_channels),
      input_channels(num_input_channels, NULL), input_buffer()
{
    for(unsigned i = 0; i < num_input_channels; i++)
        input_buffer.push_back(std::vector<SAMPLE>(BLOCK_SIZE));
}


void AudioConsumer::set_input_channel(Channel* channel, unsigned channel_i)
{
    lock.lock();
    input_channels[channel_i] = channel;
    lock.unlock();
}


void AudioConsumer::remove_channel(unsigned channel_i)
{
    lock.lock();
    input_channels[channel_i] = NULL;
    lock.unlock();
}


unsigned AudioConsumer::get_channel_index(Channel* channel)
{
    for(unsigned i = 0; i < num_input_channels; i++)
    {
        if(input_channels[i] == channel)
            return i;
    }

    throw ChannelNotFound();
}


void AudioConsumer::consume_inputs()
{
    // Read in each channel
    lock.lock();
    for(unsigned i = 0; i < num_input_channels; i++)
    {
        // If there is no channel currently, read in silence
        if(input_channels[i] == NULL)
        {
            std::cerr << "The requested channel is not connected" << std::endl;
            for(unsigned j = 0; j < BLOCK_SIZE; j++)
                input_buffer[i][j] = 0.0;
        }
        else
        {
            input_channels[i]->get_block(input_buffer[i], next_t);
        }
    }
    lock.unlock();

    // Process
    process_inputs(input_buffer);
    next_t += BLOCK_SIZE;
}


unsigned AudioConsumer::get_num_input_channels()
{
    return num_input_channels;
}




AudioFilter::AudioFilter(unsigned in_num_input_channels,
        unsigned in_num_output_channels)
    : AudioGenerator(in_num_output_channels),
      AudioConsumer(in_num_input_channels)
{}


void AudioFilter::generate_outputs(std::vector< std::vector<SAMPLE> >& outputs)
{
    consume_inputs();
}


void AudioFilter::process_inputs(std::vector< std::vector<SAMPLE> >& inputs)
{
    filter(inputs, output_buffer);
}




FilterBank::FilterBank(unsigned in_num_output_channels,
                       unsigned in_num_input_channels)
    : num_input_channels(in_num_input_channels),
      num_output_channels(in_num_output_channels),
      output_channels() {}


Channel* FilterBank::get_output_channel(int i)
{
    return output_channels[i];
}


const unsigned FilterBank::get_num_output_channels()
{
    return num_output_channels;
}


const unsigned FilterBank::get_num_input_channels()
{
    return num_input_channels;
}
