#ifndef REVERB_H
#define REVERB_H

#include "audio_generics.h"
#include "basic_elements.h"
#include "convolve.h"


namespace ClickTrack
{
    /* A simple IIR reverb filter.
     * TODO: redesign
     */
    class SimpleReverb : public AudioFilter
    {
        public:
            SimpleReverb(float decay_time, float gain, float wetness,
                         unsigned num_channels);

        private:
            void filter(std::vector<SAMPLE>& input,
                    std::vector<SAMPLE>& output, unsigned long t);

            float gain;
            float wetness;
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
    typedef struct {
        unsigned num_samples;
        SAMPLE* left;
        SAMPLE* right;
    } impulse_pair;
    impulse_pair* impulse_from_wav(const char* filename);
}

#endif
