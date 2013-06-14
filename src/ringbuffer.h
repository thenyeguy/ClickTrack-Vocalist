#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <exception>


namespace ringbuffer {
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

    
    template <class SampleT>
    class RingBuffer
    {
        private:
            int start;
            int end;
            int size;

            int buffer_size;
            SampleT* samples;
        
        public:
            RingBuffer(int n_buffer_size=1024)
            {
                start = 0;
                end = 0;
                size = 0;

                buffer_size = n_buffer_size;
                samples = new SampleT[buffer_size];
            }

            ~RingBuffer()
            {
                delete samples;
            }


            bool is_empty()
            {
                return size == 0;
            }

            int get_size()
            {
                return size;
            }


            void enq(SampleT s)
            {
                if(size == buffer_size)
                    throw new FullRingBufferException();

                samples[end] = s;
                end = (end+1) % buffer_size;
                size++;
            }

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
