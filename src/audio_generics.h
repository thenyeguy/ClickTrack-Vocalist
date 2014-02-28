#ifndef AUDIOGENERICS_H
#define AUDIOGENERICS_H

#include <vector>
#include <mutex>
#include "portaudio_wrapper.h"
#include "ringbuffer.h"


namespace ClickTrack
{
    /* This determines the size of our ring buffers and processing blocks.
     * For best performance, block size should be an integer multiple of the
     * portaudio buffer size. For processing safety the buffer size should be
     * an integer multiple of  block size, greater than 1.
     */
    const unsigned FRAME_SIZE = PORTAUDIO_BUFFER_SIZE;
    const unsigned DEFAULT_RINGBUFFER_SIZE = 4*FRAME_SIZE;


    class ChannelOutOfRange: public std::exception
    {
        virtual const char* what() const throw()
        {
            return "The requested filter does not have this many output channels.";
        }
    };
    class NoEmptyInputChannel: public std::exception
    {
        virtual const char* what() const throw()
        {
            return "This filter cannot accept further input channels.";
        }
    };
    class ChannelNotFound: public std::exception
    {
        virtual const char* what() const throw()
        {
            return "This filter does not contained the specified input channel.";
        }
    };


    /* An output channel is the basic unit with which an object receives audio.
     * It is contained within an AudioGenerator object, and serves to pipe audio
     * from its parent generator into a buffer that a later element can access.
     *
     * Contains boilerplate code to lazily update its output buffer when
     * requested.
     */
    class AudioGenerator;
    class AudioConsumer;
    class Channel
    {
        friend class AudioGenerator;
        friend class AudioConsumer;

        public:
            /* Fills an incoming buffer with one block worth of audio data
             * beginning at the requested time.
             */
            void get_frame(std::vector<SAMPLE>& buffer, unsigned t);

        protected:
            /* A channel can only exist within an audio generator, so protect
             * the constructor
             */
            Channel(AudioGenerator& in_parent, unsigned start_t=0);

            /* Fills this Channel's internal buffer with a new block of
             * audio data.
             */
            void push_frame(const std::vector<SAMPLE>& buffer);

            /* Internal state
             */
            AudioGenerator& parent;

            unsigned long start_t;
            unsigned long end_t;
            RingBuffer<SAMPLE> out;
    };


    /* An audio generator is a basic signal chain element. It must have the
     * ability to write out audio data into an output channel.
     *
     * EG a microphone is a generator.
     */
    class AudioGenerator
    {
        friend class Channel;

        public:
            AudioGenerator(unsigned num_output_channels = 1);
            virtual ~AudioGenerator() {}

            /* Returns the requested output channel by number
             */
            unsigned get_num_output_channels();
            Channel* get_output_channel(unsigned i = 0);

        protected:
            /* Writes outputs into the buffer. Calls generate_outputs to
             * determine what to write out. Used by the output channel
             */
            void fill_output_buffers();

            /* When called, updates the output channels with one more block of
             * audio data. Must be overwritten in subclasses.
             */
            virtual void generate_outputs(
                    std::vector< std::vector<SAMPLE> >& outputs) = 0;

            /* Starting time of next block
             */
            unsigned long next_out_t;

            /* Information about our internal output channels
             */
            const unsigned num_output_channels;
            std::vector<Channel> output_channels;

            /* statically allocated output buffer for speed
             */
            std::vector< std::vector<SAMPLE> > output_buffer;
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
            AudioConsumer(unsigned num_input_channels = 1);
            virtual ~AudioConsumer() {}

            /* Funtions to connect and disconnect channels. You can also look up
             * a channel's index by value, so that it can be removed and
             * restored when switching components
             */
            unsigned get_num_input_channels();
            void set_input_channel(Channel* channel, unsigned channel_i = 0);
            void remove_channel(unsigned channel_i);

            unsigned get_channel_index(Channel* channel);

            /* When called, reads in the next block from the input channels
             * and calls the process_inputs function.
             */
            void consume_inputs();

        protected:
            /* The lock is used to prevent computing outputs and modifying the
             * channels at the same time
             */
            std::mutex lock;

            /* When called on input data, processes it. Must be overwritten in
             * subclass.
             */
            virtual void process_inputs(
                    std::vector< std::vector<SAMPLE> >& inputs) = 0;

            /* Starting time of next block
             */
            unsigned long next_t;

            /* Information about our internal input channels
             */
            const unsigned num_input_channels;
            std::vector<Channel*> input_channels;

            /* statically allocated output buffer for speed
             */
            std::vector< std::vector<SAMPLE> > input_buffer;
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
            AudioFilter(unsigned num_input_channels = 1,
                    unsigned num_output_channels = 1);
            virtual ~AudioFilter() {}

        protected:
            /* Override the generator. When requested, use the consumer to
             * generate the next block of data.
             */
            void generate_outputs(std::vector< std::vector<SAMPLE> >& outputs);

            /* Override the consumer. Consumes the input and calls the filter
             * operation, then writes this to the outputs.
             */
            void process_inputs(std::vector< std::vector<SAMPLE> >& inputs);

            /* Given a list of input channel data, generate the list of output
             * channel data. Must be overwritten in subclass.
             */
            virtual void filter(std::vector< std::vector<SAMPLE> >& input, 
                    std::vector< std::vector<SAMPLE> >& output) = 0;
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
            virtual ~FilterBank() {} 

            /* Returns the requested output channel by number
             */
            Channel* get_output_channel(int i = 0);

            const unsigned get_num_output_channels();
            const unsigned get_num_input_channels();

            /* The following functions must be overriden to allow the
             * setting and removal of IO channels
             */
            virtual void set_input_channel(Channel* channel,
                    unsigned channel_i = 0) = 0;
            virtual void remove_channel(unsigned channel_i) = 0;

            virtual unsigned get_channel_index(Channel* channel) = 0;

        protected:
            const unsigned num_input_channels;
            const unsigned num_output_channels;
            std::vector<Channel*> output_channels;
    };
}

#endif
