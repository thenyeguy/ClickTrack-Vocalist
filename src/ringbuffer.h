#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <iostream>
#include <exception>
#include <vector>


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
            unsigned end_t;   // latest time still in the buffer
            unsigned size;    // number of samples currently in buffer

            unsigned buffer_size;  // the number of elements in the buffer
            std::vector<SampleT> samples; // the actual ring array
        
        public:
            /* The constructor allocates the ring array. If no size is
             * specified, it can contain at most 1024 elements, otherwise it can
             * contain as many as its argument allows.
             */
            RingBuffer(unsigned n_buffer_size=1024)
                : samples(n_buffer_size)
            {
                start_t = 0;
                end_t = 0;
                size = 0;

                buffer_size = n_buffer_size;
            }

            /* Allows you to ask for values in the currently available time
             * range of the buffer. If you try to access a time point not
             * available in the current range, throws exception.
             */
            SampleT get(const unsigned t)
            {
                if(t < start_t || t >= end_t)
                    throw RingBufferOutOfRange();

                return samples[t % buffer_size];                
            }

            /* Copies a range of values into a provided buffer. If you try to
             * access any time points not available in the current range, throws
             * exception.
             */
            void get_range(SampleT* buffer, const unsigned start,
                                            const unsigned end)
            {
                for(int i = 0; i < end-start; i++)
                    buffer[i] = get(i+start);
            }

            /* Adds a sample as the next time step in the buffer. May overwrite
             * the oldest time step. Returns the timestamp of the added value.
             */
            unsigned add(SampleT s)
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

            /* Exposes a reference to an element in the buffer so that you can
             * manually write to and modify a buffer point.
             */
            SampleT& operator[] (unsigned t)
            {
                if(t < start_t || t >= end_t)
                    throw RingBufferOutOfRange();

                return samples[t % buffer_size];                
            }

            /* Getters to expose the lowest and high timestamp
             */
            unsigned get_lowest_timestamp()
            {
                return start_t;
            }
            unsigned get_highest_timestamp()
            {
                return end_t;
            }
    };
}


#endif
