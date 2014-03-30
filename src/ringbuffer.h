#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <exception>
#include <cstdio>
#include <vector>


namespace ClickTrack{
    /* Ringbuffer is a template for a generic circular buffer queue. It contains
     * a single statically sized ring, and it can seamlessly wrap around the
     * edge. It appears to be of infinite length as long as you never have more
     * than buffer_size items inside.
     */
    template <class SampleT>
    class RingBuffer
    {
        public:
            /* The constructor allocates the ring array. It can contain as many
             * as its argument allows.
             */
            RingBuffer(unsigned n_buffer_size);

            /* Allows you to ask for values in the currently available time
             * range of the buffer. If you try to access a time point not
             * available in the current range, throws exception.
             */
            inline SampleT& operator[] (const unsigned long t);
            inline SampleT get(const unsigned long t);

            /* Copies a range of values into a provided buffer. If you try to
             * access any time points not available in the current range, throws
             * exception.
             */
            void get_range(std::vector<SampleT>& buffer, 
                    const unsigned long start, const unsigned long end);

            /* Adds a sample as the next time step in the buffer. May overwrite
             * the oldest time step. Returns the timestamp of the added value.
             */
            inline unsigned long add(SampleT s);

            /* Getters to expose the lowest and high timestamp
             */
            inline unsigned long get_lowest_timestamp();
            inline unsigned long get_highest_timestamp();

            /* WARNING: This function should not be used in ordinary
             * circumstances. It exists only for adding a new buffer during
             * runtime.
             */
            void set_new_startpoint(unsigned t);

        private:
            unsigned long start_t; // earliest time still in the buffer
            unsigned long end_t;   // latest time still in the buffer
            unsigned long size;    // number of samples currently in buffer

            unsigned long buffer_size;  // the number of elements in the buffer
            std::vector<SampleT> samples; // the actual ring array
    };


    /* Define an exceptions for use in RingBuffer. Thrown when requesting a time
     * value that is not in the ring buffer.
     */
    class RingBufferOutOfRange: public std::exception
    {
        public:
        RingBufferOutOfRange(unsigned long in_start, unsigned long in_end,
                unsigned long in_t)
            : start(in_start), end(in_end), t(in_t){}

        unsigned long start, end, t;
        virtual const char* what() const throw()
        {
            char* buffer = new char[256];
            sprintf(buffer, "%s\n    Buffer range=[%lu-%lu), t=%lu\n",
                    "The requested time value is not stored in the buffer.",
                    start, end, t);
            return buffer;
        }
    };
}

#include "ringbuffer.cpp"

#endif
