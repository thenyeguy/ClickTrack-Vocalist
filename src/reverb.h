#ifndef REVERB_H
#define REVERB_H

#include "audio_generics.h"
#include "basic_elements.h"
#include "convolve.h"


namespace ClickTrack
{
    /* A simple algorithmic reverb based on work by Moorer
     * For explanation of the filter design, see "About This Reverberation
     * Business" by James Moorer.
     *
     * The room selection lets you change the reverb among a set of presets.
     *      HALL: Based on Boston Symphony Hall, by Moorer
     */
    class MoorerReverb : public AudioFilter
    {
        public:
            /* room is chosen from the enum below.
             * rev_time is the -60dB time, in milliseconds
             * gain is the gain on the entire reverb, in raw amplitude
             * wetness is the percent reverb in the signal, between 0 and 1
             */
            enum Room { HALL };
            MoorerReverb(Room room, float rev_time, float gain, float wetness,
                         unsigned num_channels);

            /* Setters for the reverb parameters. Currently, the room itself
             * can't be changed after initialization
             */
            void set_rev_time(float rev_time);
            void set_gain(float gain);
            void set_wetness(float wetness);

        private:
            /* The following are used to reinitialize coefficients when
             * parameters change
             */
            void set_comb_filter_gains();
            void allocate_ringbuffers();

            /* The filter function 
             */
            void filter(std::vector<SAMPLE>& input,
                    std::vector<SAMPLE>& output, unsigned long t);

            /* Filter parameters
             */
            Room room;
            float rev_time;
            float gain;
            float wetness;

            /* Tapped delay line. The delays are assumed to be in ascending
             * order, and the gains are matched to the delays.
             *
             * tapped_delay_lines is a vector of ringbuffers, one for each
             * output channel
             */
            std::vector< RingBuffer<SAMPLE> > tapped_delay_lines;
            std::vector<unsigned> tapped_delays;
            std::vector<float>    tapped_gains;

            /* Comb filter. The delays and gains are assumed to be matched.
             *
             * comb_delay_lines is a vector of vectors of ringbuffers. The outer
             * vector is for each channel, the inner vector is for each comb
             * filter
             */
            std::vector< std::vector< RingBuffer<SAMPLE> > > comb_delay_lines;
            std::vector<unsigned> comb_delays;
            std::vector<float>    comb_gains;

            /* Combination parameters
             *
             * The output buffers are vectors of ringbuffers, one for each
             * output channel
             */
            std::vector< RingBuffer<SAMPLE> > tapped_delay_line_outputs;
            std::vector< RingBuffer<SAMPLE> > comb_outputs;
            unsigned tapped_out_delay;
            unsigned comb_out_delay;
            float    comb_out_gain;
    };


    /* A simple one channel convolution reverb filter.
     * TODO: replace when convolution reverb is fixed
     */
/*
 *     class ConvolutionReverb : public FilterBank
 *     {
 *         public:
 *             ConvolutionReverb(unsigned impulse_length, SAMPLE* impulse,
 *                    float gain, float wetness);
 * 
 *             void set_input_channel(Channel* channel,
 *                     unsigned channel_i = 0);
 *             void remove_channel(unsigned channel_i = 0);
 * 
 *             unsigned get_channel_index(Channel* channel);
 * 
 *         private:
 *             float gain;
 *             float wetness;
 *             
 *             ConvolutionFilter conv; 
 *             GainFilter wet;
 *             GainFilter dry;
 *             Adder out;
 *     };
 */


    /* Given a wav file, reads that wav file into a pair of arrays. The two
     * arrays represent the impulse response given in the left and right
     * channels.
     */
    /*
     * typedef struct {
     *     unsigned num_samples;
     *     SAMPLE* left;
     *     SAMPLE* right;
     * } impulse_pair;
     * impulse_pair* impulse_from_wav(const char* filename);
     */
}

#endif
