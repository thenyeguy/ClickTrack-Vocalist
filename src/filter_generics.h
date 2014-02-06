#ifndef FILTERGENERICS_H
#define FILTERGENERICS_H

#include <vector>
#include "portaudio_wrapper.h"
#include "ringbuffer.h"


namespace FilterGenerics
{
    /* This determines the size of our ring buffers and processing blocks.
     * For best performance, block size should be an integer multiple of the
     * portaudio buffer size. For processing safety the buffer size should be
     * an integer multiple of  block size, greater than 1.
     */
    const unsigned DEFAULT_BLOCK_SIZE = Portaudio::DEFAULT_BUFFER_SIZE;
    const unsigned DEFAULT_RINGBUFFER_SIZE = 4*DEFAULT_BLOCK_SIZE;


    /* An output channel is the basic unit with which an object receives audio.
     * It is contained within an AudioGenerator object, and serves to pipe audio
     * from its parent generator into a buffer that a later element can access.
     *
     * Contains boilerplate code to lazily update its output buffer when
     * requested.
     */
    class AudioGenerator;
    class OutputChannel
    {
        friend class AudioGenerator;

        public:
            /* Fills an incoming buffer with one block worth of audio data
             * beginning at the requested time.
             */
            void get_block(SAMPLE* buffer, unsigned t);

        private:
            // Called only by parent AudioGenerator
            OutputChannel(AudioGenerator* in_parent);

            /* Fills the OutputChannel's internal buffer with a new block of
             * audio data.
             */
            void push_block(const SAMPLE* buffer);


            AudioGenerator* parent;

            unsigned start_t;
            unsigned end_t;
            ClickTrackUtils::RingBuffer<SAMPLE> out;
    };


    /* An audio generator is a basic signal chain element. It must have the
     * ability to write out audio data into an output channel.
     *
     * EG a microphone is a generator.
     */
    class AudioGenerator
    {
        friend class OutputChannel;

        public:
            AudioGenerator(unsigned in_num_output_channels);
            ~AudioGenerator();

            /* Returns the requested output channel by number
             */
            OutputChannel* get_output_channel(int i = 0);
            const unsigned get_num_output_channels();

        protected:
            /* Writes outputs into the buffer. Calls generate_outputs to
             * determine what to write out. Used by the output channel
             */
            void write_outputs();

            /* When called, updates the output channels with one more block of
             * audio data.
             *
             * Must be overwritten in subclasses.
             */
            virtual void generate_outputs(SAMPLE** outputs) = 0;


            const unsigned num_output_channels;
            std::vector<OutputChannel> output_channels;

            // statically allocated output buffer for speed
            SAMPLE** output_buffer;
    };


    /* An audio consumer is a basic signal chain element. It must have the
     * ability to read in audio data from an output channel, and perform an
     * operation on that input data.
     *
     * EG a speaker is a consumer.
     */
    class AudioConsumer
    {
        public:
            AudioConsumer(unsigned in_num_input_channels,
                          OutputChannel** in_input_channels);
            ~AudioConsumer();

            /* When called, reads in the next block from the input channels
             * and calls the process_inputs function.
             */
            void consume_inputs();

            const unsigned get_num_input_channels();

        protected:
            /* When called on input data, processes it. Must be overwritten in
             * subclass.
             */
            virtual void process_inputs(SAMPLE** inputs) = 0;


            unsigned next_t; // Starting time unit of next block

            const unsigned num_input_channels;
            std::vector<OutputChannel*> input_channels;

            // statically allocated input buffer for speed
            SAMPLE** input_buffer;
    };


    /* The AudioFilter is a hybrid signal chain element. It both consumes and
     * generates audio data, and acts as a pure function that transforms input
     * audio to output audio.
     *
     * EG a reverb is a filter
     */
    class AudioFilter : public AudioGenerator, public AudioConsumer
    {
        public:
            AudioFilter(unsigned in_num_output_channels,
                        unsigned in_num_input_channels,
                        OutputChannel** in_input_channels);
            virtual ~AudioFilter();

        protected:
            /* Override the generator. When requested, use the consumer to
             * generate the next block of data.
             */
            void generate_outputs(SAMPLE** outputs);

            /* Override the consumer. Consumes the input and calls the filter
             * operation, then writes this to the outputs.
             */
            void process_inputs(SAMPLE** inputs);

            /* Given a list of input channel data, generate the list of output
             * channel data. Must be overwritten in subclass.
             */
            virtual void filter(SAMPLE** input, SAMPLE** output) = 0;
    };


    /* The FilterBank is a hybrid signal chain element. It both consumes and
     * generates audio, but does so by internally connecting many filters
     * together.
     *
     * To implement a FilterBank, one must initialize their elements in the
     * constructor, as well as insert their output channels into the vector.
     */
    class FilterBank
    {
        public:
            FilterBank(unsigned in_num_output_channels,
                       unsigned in_num_input_channels);
        
            /* Returns the requested output channel by number
             */
            OutputChannel* get_output_channel(int i = 0);

            const unsigned get_num_output_channels();
            const unsigned get_num_input_channels();

        protected:
            const unsigned num_input_channels;
            const unsigned num_output_channels;
            std::vector<OutputChannel*> output_channels;
    };
}

#endif
