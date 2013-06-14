#ifndef FILTERBLOCK_H
#define FILTERBLOCK_H
#include "portaudio_wrapper.h"
#include "ringbuffer.h"


class FilterBlock
{
    protected:
        SAMPLE* in_buffer;
    public:
        RingBuffer<SAMPLE> out_buffer;

        virtual void fill_in_buffer(RingBuffer<SAMPLE>* input)
        {
            int i = 0;
            while(!input->is_empty())
            {
                in_buffer[i] = input.deq();
                i++;
            }
        }

        virtual void fill_out_buffer() = 0;
}


#endif
