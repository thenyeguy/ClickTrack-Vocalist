#ifndef CONVOLVE_H
#define CONVOLVE_H

#include <vector>
#include "ringbuffer.h"
#include "filter_generics.h"
#include "fft.h"

using namespace FilterGenerics;


namespace Filters
{
    /* The ConvolutionFilter implements a real time convolution of the input
     * signal with a precomputed impulse response.
     *
     * Uses the overlap add method to keep a running buffer of the result.
     * Also uses overlap add to calculate the convolution of each time block
     * with the impulse response.
     *
     * Computes circular convolution using the property that convolution in time
     * is equivalent to multiplication in frequency. Uses linearity of the FFT
     * to only perform a single transform into and out of frequency domain.
     */
    const unsigned fft_multiplier = 16;
    class ConvolutionFilter : public AudioFilter
    {
        public:
            ConvolutionFilter(unsigned impulse_length,
                              SAMPLE* in_impulse_response);
            ~ConvolutionFilter();

        private:
            void filter(std::vector< std::vector<SAMPLE> >& input, 
                    std::vector< std::vector<SAMPLE> >& output);

            const unsigned transform_size;
            FFT::Transformer transformer;

            const unsigned num_impulse_blocks;
            std::vector<complex<SAMPLE>*> impulse_response;

            ClickTrackUtils::RingBuffer<complex<SAMPLE>*> frequency_buffer;
            ClickTrackUtils::RingBuffer<SAMPLE> reverb_buffer;


            // Preallocate buffers for speed
            complex<SAMPLE>* input_buffer;
            complex<SAMPLE>* output_buffer;
    };
}

#endif
