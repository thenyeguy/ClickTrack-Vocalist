#ifndef REVERB_H
#define REVERB_H

#include "filter_generics.h"
#include "elementary_filters.h"
#include "convolve.h"

using namespace FilterGenerics;

namespace Filters
{
    /* A simple IIR reverb filter.
     */
    class SimpleReverb : public AudioFilter
    {
        public:
            SimpleReverb(float decay_time, float gain, float wetness,
                         unsigned num_channels, OutputChannel** input_channels);
            ~SimpleReverb();

        private:
            void filter(SAMPLE** input, SAMPLE** output);

            std::vector<SAMPLE> sample_rollover;

            float gain;
            float wetness;
            float decay_ratio;
    };


    /* A simple one channel convolution reverb filter.
     */
    class ConvolutionReverb : public FilterBank
    {
        public:
            ConvolutionReverb(unsigned impulse_length, SAMPLE* impulse,
                   float gain, float wetness,
                   OutputChannel* in_input_channel);
            ~ConvolutionReverb();

        private:
            float gain;
            float wetness;
            
            ConvolutionFilter* conv; 
            GainFilter* wet;
            GainFilter* dry;
            Adder* out;
    };


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
