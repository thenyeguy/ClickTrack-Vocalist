#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <exception>


namespace ringbuffer {
    /* Define two exceptions for use in RingBuffer. Thrown when a ring buffer is
     * required to be nonempty or nonfull.
     */
    class EmptyRingBufferException: public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Can't dequeue from an empty buffer";
        }
    };
    class FullRingBufferException: public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Can't enqueue to a full buffer";
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
            int start; // starting index of the queue in the ring
            int end;   // ending index of the queue in the ring
            int size;  // the number of elements contained in the ring

            int buffer_size;  // the max size of the ring
            SampleT* samples; // the actual ring array
        
        public:
            /* The constructor allocates the ring array. If no size is
             * specified, it can contain at most 1024 elements, otherwise it can
             * contain as many as its argument allows.
             */
            RingBuffer(int n_buffer_size=1024)
            {
                start = 0;
                end = 0;
                size = 0;

                buffer_size = n_buffer_size;
                samples = new SampleT[buffer_size];
            }

            /* Deconstructor. Frees the memory allocated to the ring.
             */
            ~RingBuffer()
            {
                delete samples;
            }

            /* Returns true iff there are currently no elements in the ring.
             */
            bool is_empty()
            {
                return size == 0;
            }

            /* Returns the number of elements in the ring that have been
             * enqueued but not yet dequeued.
             */
            int get_size()
            {
                return size;
            }

            /* Enqueue takes a single sample and puts it at the end of the ring.
             * If the ring is already full, throws FullRingBufferException.
             */
            void enq(SampleT s)
            {
                if(size == buffer_size)
                    throw new FullRingBufferException();

                samples[end] = s;
                end = (end+1) % buffer_size;
                size++;
            }

            /* Dequeue removes a single sample from the beginning of the ring.
             * After a dequeue, the head of the ring is advanced by one.
             * If the ring is already empty, throws EmptyRingBufferException
             */
            SampleT deq()
            {
                if(size == 0)
                    throw new EmptyRingBufferException();

                SampleT s = samples[start];
                start = (start+1) % buffer_size;
                size--;
                return s;
            }
    };
}


#endif
