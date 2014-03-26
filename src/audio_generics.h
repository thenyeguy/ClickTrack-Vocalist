#ifndef AUDIOGENERICS_H
#define AUDIOGENERICS_H

#include <vector>
#include "portaudio_wrapper.h"
#include "ringbuffer.h"


namespace ClickTrack
{
    /* This determines the size of our internal ring buffers.
     */
    const unsigned DEFAULT_RINGBUFFER_SIZE = BUFFER_SIZE;


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
        friend class AudioFilter;

        public:
            /* Fills an incoming buffer with one block worth of audio data
             * beginning at the requested time.
             */
            SAMPLE get_sample(unsigned long t);

        protected:
            /* A channel can only exist within an audio generator, so protect
             * the constructor
             */
            Channel(AudioGenerator& in_parent, unsigned long start_t=0);

            /* Adds a sample to this Channel's internal buffer 
             */
            void push_sample(SAMPLE s);

            /* Internal state
             */
            AudioGenerator& parent;
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

            /* Getters for output channels
             */
            unsigned get_num_output_channels();
            Channel* get_output_channel(unsigned i = 0);

        protected:
            /* When called, updates the output channels with one more frame of
             * audio at time t.
             *
             * Must be overwritten in subclasses.
             */
            virtual void generate_outputs(std::vector<SAMPLE>& outputs, 
                    unsigned long t) = 0;

            /* Returns the next sample time
             */
            unsigned long get_next_time();

        private:
            /* Writes outputs into the buffer. Calls tick to determine what to
             * write out. Used by the output channel
             */
            void generate();

            /* Starting time of next block
             */
            unsigned long next_out_t;

            /* Information about our internal output channels
             */
            std::vector<Channel> output_channels;

            /* Statically allocated frame for speed
             */
            std::vector<SAMPLE> output_frame;
    };


    /* An audio consumer is a basic signal chain element. It must have the
     * ability to read in audio data from an output channel, and perform an
     * operation on that input data.
     *
     * A consumer can optionally trigger a callback function whenever it
     * consumes its outputs.
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

            /* When called, reads in the next frame from the input channels
             * and calls the tick function.
             */
            void consume();

        protected:
            /* When called on input data, processes it. Must be overwritten in
             * subclass.
             */
            virtual void process_inputs(std::vector<SAMPLE>& inputs, 
                    unsigned long t) = 0;

            /* Returns the next sample time
             */
            unsigned long get_next_time();

        private:
            /* Starting time of next block
             */
            unsigned long next_in_t;

            /* Information about our internal input channels
             */
            std::vector<Channel*> input_channels;

            /* statically allocated frame for speed
             */
            std::vector<SAMPLE> input_frame;
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
            /* Given an input frame, generate a frame of output data. Must be
             * overwritten in subclass.
             */
            virtual void filter(std::vector<SAMPLE>& input, 
                    std::vector<SAMPLE>& output, unsigned long t) = 0;

            /* Gets the next sample time from the consumer
             */
            using AudioConsumer::get_next_time;

        private:
            /* Override the tick functions. When requested, use our tick to
             * generate the next frame of data.
             */
            void generate_outputs(std::vector<SAMPLE>& inputs, unsigned long t);
            void process_inputs(std::vector<SAMPLE>& outputs, unsigned long t);

            /* Statically allocated frame for speed. Seperate from the
             * generator's output to maintain clean interface
             */
            std::vector<SAMPLE> output_frame;
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
            FilterBank(unsigned num_output_channels,
                       unsigned num_input_channels);
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


    /* Exceptions used by the audio generics
     */
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
}

#endif
