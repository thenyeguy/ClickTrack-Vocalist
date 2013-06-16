#ifndef FILTERBLOCK_H
#define FILTERBLOCK_H
#include <vector>
#include "portaudio_wrapper.h"
#include "ringbuffer.h"


namespace FilterChain {
    /* This determines the size of our ring buffers and processing blocks.
     * For best performance, block size should be an integer multiple of the
     * portaudio buffer size. For processing safety the buffer size should be
     * an integer multiple of the block size, greater than 1.
     */
    const unsigned DEFAULT_BLOCK_SIZE = Portaudio::DEFAULT_BUFFER_SIZE;
    const unsigned DEFAULT_BUFFER_SIZE = 4*DEFAULT_BLOCK_SIZE;


    /* A audio element is the most generic unit in an audio chain.
     *
     * To implement a standard audio element, all that is nessecary is defining
     * the virtual method generate_output. The top level class will handle the
     * boilerplate code to determine when to processs.
     */
    class AudioElement
    {
        protected:
            /* Stores its output into a ring buffer. Each sample is an array of
             * channels.
             */
            unsigned num_output_channels;
            std::vector< ClickTrackUtils::RingBuffer<SAMPLE> > out_buffers;

            // Used to determine whether we need to compute new outputs.
            unsigned next_block;

        public:
            AudioElement(unsigned num_channels = 1);

            /* Given a channel number and a time block id, returns a 1D array of
             * samples for that channel. Automatically evaluates new output if
             * nessecary.
             */
            SAMPLE* get_sample_block(unsigned output_channel, unsigned block_id);

            /* To implement an audio element, one must override this. The
             * generate_output function must fill the out_buffer with the next
             * block of samples.
             */
            virtual void generate_output() = 0;
    };


    /* A filter element is a pure function - it takes a variable number of
     * inputs and outputs to a variable number of channels. To implement a new
     * filter, one must only override the run_filter function to perform your
     * filter. The function will automatically pull in the input data.
     */
    class FilterElement: public AudioElement
    {
        protected:
            unsigned num_inputs;
            unsigned* input_channels;
            AudioElement* inputs;
        public:
            FilterElement(AudioElement* input_elements,
                          unsigned* input_chans,
                          unsigned num_input_elements = 1,
                          unsigned num_channels = 1);

            /* This function automatically pulls in inputs, then runs filter.
             */
            using AudioElement::generate_output;
            void generate_output(unsigned block_id);

            /* Actually runs the filter, and stores it to output buffer. Takes
             * input as a list of samples, each of which is a list of channels.
             */
            virtual void run_filter(SAMPLE** inputs) = 0;
    };


    /* An output element listens to a variable number of inputs, and pipes the
     * output outside the ClickTrack environment. To create a new output
     * element, override the output function.
     */
    class OutputElement
    {
        protected:
            unsigned num_inputs;
            unsigned* input_channels;
            AudioElement* inputs;

            unsigned next_block;
        public:
            OutputElement(AudioElement* input_elements, unsigned* input_chans,
                          unsigned num_input_elements = 1);

            /* Fetches our inputs, then calls the output function.
             */
            void process_inputs();

            /* Actually outputs our data.
             */
            virtual void output(SAMPLE** inputs) = 0;
    };
}


#endif
