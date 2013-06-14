#ifndef FILTERBLOCK_H
#define FILTERBLOCK_H
#include "portaudio_wrapper.h"
#include "ringbuffer.h"


namespace filterblocks {
    /* A filter block is the generic building block for a signal processing
     * chain. All real filters inherit from this interface.
     *
     * To be a valid filter block, a filter must adhere to two properties:
     *   (1) on fill_in_buffer, it must populate its input buffer from the
     *       previous element in the chain. An object may make up its input if
     *       it is the first element (ie a microphone input).
     *   (2) on fill_out_buffer, it must use the in_buffer to calculate the
     *       output of the filter, and store it in the output RingBuffer so that
     *       the next element in the chain can access it. If this is the last
     *       element, it may just throw into space (ie a speaker output).
     */
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
}


#endif
