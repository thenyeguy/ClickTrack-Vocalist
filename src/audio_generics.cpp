#include <iostream>
#include "audio_generics.h"


using namespace ClickTrack;


Channel::Channel(AudioGenerator& in_parent, unsigned long in_start_t)
    : parent(in_parent), out(DEFAULT_RINGBUFFER_SIZE)
{
    out.set_new_startpoint(in_start_t);
}


SAMPLE Channel::get_sample(unsigned long t)
{
    // If this block already fell out of the buffer, just return silence
    if(out.get_lowest_timestamp() > t)
    {
        std::cerr << "Channel has requested a time older than is in "
            << "its buffer." << std::endl;
        return 0.0;
    }

    // Otherwise generate enough audio
    while(out.get_highest_timestamp() <= t)
        parent.generate();

    return out[t];
}


void Channel::push_sample(SAMPLE s)
{
    out.add(s);
}




AudioGenerator::AudioGenerator(unsigned in_num_output_channels)
    : next_out_t(0), output_channels(), output_frame()
{
    for(unsigned i = 0; i < in_num_output_channels; i++)
    {
        output_channels.push_back(Channel(*this));
        output_frame.push_back(0.0);
    }
}


unsigned AudioGenerator::get_num_output_channels()
{
    return output_channels.size();
}


Channel* AudioGenerator::get_output_channel(unsigned i)
{
    if(i >= output_channels.size())
        throw ChannelOutOfRange();
    return &output_channels[i];
}


void AudioGenerator::generate()
{
    generate_outputs(output_frame, next_out_t);
    next_out_t++;

    //Write the outputs into the channel
    for(int i = 0; i < output_channels.size(); i++)
        output_channels[i].push_sample(output_frame[i]);
}


unsigned long AudioGenerator::get_next_time()
{
    return next_out_t;
}




AudioConsumer::AudioConsumer(unsigned in_num_input_channels)
    : callback(NULL), payload(NULL), next_in_t(0), 
    input_channels(in_num_input_channels, NULL), input_frame()
{
    for(unsigned i = 0; i < in_num_input_channels; i++)
        input_frame.push_back(0.0);
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
    for(unsigned i = 0; i < input_channels.size(); i++)
    {
        if(input_channels[i] == channel)
            return i;
    }

    throw ChannelNotFound();
}


unsigned AudioConsumer::get_num_input_channels()
{
    return input_channels.size();
}


void AudioConsumer::consume()
{
    // Read in each channel
    lock.lock();
    for(unsigned i = 0; i < input_channels.size(); i++)
    {
        // If there is no channel currently, read in silence
        if(input_channels[i] == NULL)
        {
            std::cerr << "The requested channel is not connected" << std::endl;
            input_frame[i] = 0.0;
        }
        else
        {
            input_frame[i] = input_channels[i]->get_sample(next_in_t);
        }
    }
    lock.unlock();

    // Process
    process_inputs(input_frame, next_in_t);
    next_in_t++;

    // Run the callback
    if(callback != NULL)
        callback(next_in_t, payload);
}


void AudioConsumer::register_callback(callback_t in_callback, void* in_payload)
{
    callback = in_callback;
    payload = in_payload;
}


unsigned long AudioConsumer::get_next_time()
{
    return next_in_t;
}




AudioFilter::AudioFilter(unsigned in_num_input_channels,
        unsigned in_num_output_channels)
    : AudioGenerator(in_num_output_channels),
      AudioConsumer(in_num_input_channels), output_frame()
{
    for(unsigned i = 0; i < in_num_output_channels; i++)
    {
        output_frame.push_back(0.0);
    }
}


void AudioFilter::generate_outputs(std::vector<SAMPLE>& outputs, unsigned long t)
{
    consume();
    
    // Copy our outputs to the final output buffer
    for(unsigned i = 0; i <= outputs.size(); i++)
        outputs[i] = output_frame[i];
}


void AudioFilter::process_inputs(std::vector<SAMPLE>& inputs, unsigned long t)
{
    filter(inputs, output_frame, t);
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
