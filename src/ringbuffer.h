#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <exception>


namespace ClickTrackUtils {
    /* Define an exceptions for use in RingBuffer. Thrown when requesting a time
     * value that is not in the ring buffer.
     */
    class RingBufferOutOfRange: public std::exception
    {
        virtual const char* what() const throw()
        {
            return "The requested time value is not stored in the buffer.";
        }
    };

    
    /* Ringbuffer is a template for a generic circular buffer queue. It contains
     * a single statically sized ring, and it can seamlessly wrap around the
     * edge. It appears to be of infinite length as long as you never have more
     * than buffer_size items inside.
     */
    template <class SampleT>
    class RingBuffer
    {
        private:
            unsigned start_t; // earliest time still in the buffer
            unsigned end_t;   //  latest time still in the buffer
            unsigned samples_added; // the number of samples stored in the buffer

            unsigned buffer_size;  // the number of elements in the buffer
            SampleT* samples; // the actual ring array
        
        public:
            /* The constructor allocates the ring array. If no size is
             * specified, it can contain at most 1024 elements, otherwise it can
             * contain as many as its argument allows.
             */
            RingBuffer(unsigned n_buffer_size=1024)
            {
                start_t = 0;
                end_t = 0;
                samples_added = 0;

                buffer_size = n_buffer_size;
                samples = new SampleT[buffer_size];
            }

            /* Deconstructor. Frees the memory allocated to the ring.
             */
            ~RingBuffer()
            {
                delete samples;
            }

            /* Adds a sample as the next time step in the buffer. May overwrite
             * the oldest time step.
             */
            void add_sample(SampleT s)
            {
                //  Write the sample into the ring
                unsigned i = end_t % buffer_size;
                samples[i] = s;
                end_t++;

                // If we are at max capacity, we replace the oldest element
                if(samples_added == buffer_size)
                    start_t++;
                else
                    samples_added++;

            }

            /* Gets the sample at time t. If this is not in the range stored
             * inside the buffer, throws OutOfRange exception.
             */
            SampleT get_sample(unsigned t)
            {
                if(t < start_t || t >= end_t)
                    throw RingBufferOutOfRange();

                return samples[t % buffer_size];
            }
    };
}


#endif
