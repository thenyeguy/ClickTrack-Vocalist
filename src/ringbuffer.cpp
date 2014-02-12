#ifndef RINGBUFFER_CPP
#define RINGBUFFER_CPP
#include "ringbuffer.h"

using namespace ClickTrack;


template <class SampleT>
RingBuffer<SampleT>::RingBuffer(unsigned n_buffer_size)
    : samples(n_buffer_size)
{
    start_t = 0;
    end_t = 0;
    size = 0;

    buffer_size = n_buffer_size;
}


template <class SampleT>
SampleT RingBuffer<SampleT>::get(const unsigned t)
{
    if(t < start_t || t >= end_t)
    {
        std::cerr << "out of range: " << start_t << " "
            << t << " " << end_t << std::endl;
        throw RingBufferOutOfRange();
    }

    return samples[t % buffer_size];                
}


template <class SampleT>
void RingBuffer<SampleT>::get_range(std::vector<SampleT>& buffer,
        const unsigned start, const unsigned end)
{
    for(int i = 0; i < end-start; i++)
        buffer[i] = get(i+start);
}


template <class SampleT>
unsigned RingBuffer<SampleT>::add(SampleT s)
{
    // Drop the earliest time if nessecary
    if(size == buffer_size)
        start_t++;
    else
        size++;

    // Write the sample into the ring
    unsigned i = end_t % buffer_size;
    samples[i] = s;
    end_t++;

    return end_t-1;
}


template <class SampleT>
SampleT& RingBuffer<SampleT>::operator[] (unsigned t)
{
    if(t < start_t || t >= end_t)
    {
        std::cerr << "out of range: " << start_t << " "
            << t << " " << end_t << std::endl;
        throw RingBufferOutOfRange();
    }

    return samples[t % buffer_size];                
}


template <class SampleT>
unsigned RingBuffer<SampleT>::get_lowest_timestamp()
{
    return start_t;
}


template <class SampleT>
unsigned RingBuffer<SampleT>::get_highest_timestamp()
{
    return end_t;
}


template <class SampleT>
void RingBuffer<SampleT>::set_new_startpoint(unsigned t)
{
    start_t = t;
    end_t = t;
    size = 0;
}

#endif
