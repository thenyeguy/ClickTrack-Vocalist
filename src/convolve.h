#ifndef CONVOLVE_H
#define CONVOLVE_H

#include "ringbuffer.h"
#include "filter_generics.h"
#include "fft.h"


namespace FilterGenerics
{
    /* The ConvolutionFilter implements a real time convolution of the input
     * signal with a precomputed impulse response.
     *
     * Uses the overlap add method to keep a running buffer of the result.
     * Computes circular convolution using the property that convolution in time
     * is equivalent to multiplication in frequency.
     */
    class ConvolutionFilter : public AudioFilter
    {
        private:
            const unsigned output_length;
            ClickTrackUtils::RingBuffer<SAMPLE> overlaps;

            FFT::Transformer transformer;
            complex<SAMPLE>* impulse_response;

            // Preallocate buffers for speed
            complex<SAMPLE>* input_buffer;
            complex<SAMPLE>* output_buffer;
            
        public:
            ConvolutionFilter(OutputChannel* in_input_channel,
                              unsigned impulse_length,
                              SAMPLE* in_impulse_response);
            ~ConvolutionFilter();

            void filter(SAMPLE** input, SAMPLE** output);
    };
}

#endif
