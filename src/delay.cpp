#include <iostream>
#include "delay.h"

using namespace FilterGenerics;
using namespace Filters;


Delay::Delay(float in_delay_time, float in_feedback, float in_wetness,
             OutputChannel** in_input_channels, unsigned in_num_channels)
    : AudioFilter(in_num_channels, in_num_channels, in_input_channels),
      delay(Portaudio::DEFAULT_SAMPLE_RATE * in_delay_time),
      feedback(in_feedback), wetness(in_wetness),
      delay_buffers()
{
    for(unsigned i = 0; i < in_num_channels; i++)
    {
        // Create new buffer and fill it with zeros for the previous time
        auto rb = new ClickTrackUtils::RingBuffer<SAMPLE>(delay);
        for(unsigned t = 0; t < delay; t++)
            rb->add(0.0);

        // Add to the buffer set
        delay_buffers.push_back(rb);
    }
}


Delay::~Delay()
{
    for(auto rb : delay_buffers)
        delete rb;
}


void Delay::set_delay(float in_delay)
{
    delay = in_delay;

    // Reset buffers to empty
    for(unsigned i = 0; i < num_input_channels; i++)
    {
        auto rb = new ClickTrackUtils::RingBuffer<SAMPLE>(delay);
        for(unsigned t = 0; t < delay; t++)
            rb->add(0.0);

        delete delay_buffers[i];
        delay_buffers[i] = rb;
    }
}


void Delay::set_feedback(float in_feedback)
{
    feedback = in_feedback;
}


void Delay::set_wetness(float in_wetness)
{
    wetness = in_wetness;
}


void Delay::filter(SAMPLE** input, SAMPLE** output)
{
    for(int i = 0; i < num_input_channels; i++)
    {
        std::cout << i << std::endl;
        for(int j = 0; j < DEFAULT_BLOCK_SIZE; j++)
        {
            // Add the delayed version of the signal in, respecting wetness
            SAMPLE delayed_value = delay_buffers[i]->get(next_t + j);
            output[i][j] = (1.0-wetness)*input[i][j] + wetness*delayed_value;

            // Scale by the feedback and delay our current input
            delay_buffers[i]->add(input[i][j] + feedback*delayed_value);
        }
    }
}
