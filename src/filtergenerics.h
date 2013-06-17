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
     * an integer multiple of the block size, greater than 1.
     */
    const unsigned DEFAULT_BLOCK_SIZE = Portaudio::DEFAULT_BUFFER_SIZE;
    const unsigned DEFAULT_BUFFER_SIZE = 4*DEFAULT_BLOCK_SIZE;


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
        private:
            unsigned channel_id;
            AudioGenerator* parent;

            unsigned start_t;
            unsigned end_t;
            ClickTrackUtils::RingBuffer<SAMPLE> out;

        public:
            // Called only by parent AudioGenerator
            OutputChannel(unsigned in_channel_id,
                          AudioGenerator* in_parent);

            /* Fills an incoming buffer with one block worth of audio data
             * beginning at the requested time.
             */
            void fill_external_buffer(SAMPLE* buffer, const unsigned t);

            /* Fills the OutputChannel's internal buffer with a new block of
             * audio data. Called only by parent AudioGenerator.
             */
            void fill_internal_buffer(const SAMPLE* buffer);
    };


    /* An audio generator is a basic signal chain element. It must have the
     * ability to write out audio data into an output channel.
     *
     * EG a microphone is a generator.
     */
    class AudioGenerator
    {
        protected:
            const unsigned num_output_channels;
            std::vector<OutputChannel> output_channels;

        public:
            AudioGenerator(const unsigned in_num_output_channels);

            /* Returns the requested output channel by number
             */
            OutputChannel& get_output_channel(int i);

            /* When called, updates the output channels with one more block of
             * audio data. Should only be called by output channels.
             *
             * Must be overwritten in subclasses.
             */
            virtual void generate_outputs() = 0;
    };


    /* An audio consumer is a basic signal chain element. It must have the
     * ability to read in audio data from an output channel, and perform an
     * operation on that input data.
     *
     * EG a speaker is a consumer.
     */
    class AudioConsumer
    {
        protected:
            unsigned next_t; // Starting time unit of next block

            unsigned num_input_channels;
            std::vector<OutputChannel*> input_channels;

        public:
            AudioConsumer(unsigned in_num_input_channels,
                          OutputChannel* in_input_channels);

            /* When called, reads in the next block from the input channels
             * and calls the process_inputs function.
             */
            void consume_inputs();

            /* When called on input data, processes it. Must be overwritten in
             * subclass.
             */
            virtual void process_inputs(SAMPLE** inputs) = 0;
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
            AudioFilter(const unsigned in_num_output_channels,
                        const unsigned in_num_input_channels,
                        OutputChannel* in_input_channels);

            /* Override the generator. When requested, use the consumer to
             * generate the next block of data.
             */
            void generate_outputs();

            /* Override the consumer. Consumes the input and calls the filter
             * operation, then writes this to the outputs.
             */
            void process_inputs(SAMPLE** inputs);

            /* Given a list of input channel data, generate the list of output
             * channel data. Must be overwritten in subclass.
             */
            virtual void filter(SAMPLE** input, SAMPLE** output) = 0;
    };
}

#endif
