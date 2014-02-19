#ifndef FFT_H
#define FFT_H

#include <complex>
#include <vector>
#include <fftw3.h>
#include "portaudio_wrapper.h"


namespace ClickTrack
{
    /* The transformer provides an efficient framework for computing fixed size
     * FFTs. Once initialized, it precomputes lookup tables for bitreversal and
     * twiddle factors. You can then utilize these to perform FFTs.
     *
     * This function was written from scratch for ClickTrack as a learning
     * exercise, but is poorly optimized and runs fairly slowly.
     */
    class Transformer
    {
        public:
            Transformer(unsigned in_size);

            /* Given the input complex numbers, computes the fft and places the
             * result in the output buffer. May destructively modify the input
             * buffer. */
            void fft(std::complex<SAMPLE>* in, std::complex<SAMPLE>* out);
            void ifft(std::complex<SAMPLE>* in, std::complex<SAMPLE>* out);

        private:
            const unsigned size;
            const unsigned buffer_size;

            std::vector<unsigned> bit_reverses;
            std::vector< std::complex<SAMPLE> > twiddles;
    };


    /* Helper function - internally used by transformer.
     * Given an integer, returns the next highest power of two.
     */
    unsigned next_power_of_two(unsigned in);

    /* Helper function - internally used by transformer.
     * Given an integer, returns the bitreversed value
     */
    unsigned bit_reverse(unsigned in);

    /* Helper function - internally used by transformer.
     * Find the log2 of an integer. Rounded up.
     */
    unsigned int_log(unsigned in);
}

#endif
