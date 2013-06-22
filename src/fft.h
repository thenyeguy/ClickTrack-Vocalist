#ifndef FFT_H
#define FFT_H

#include <complex>
#include <vector>
#include "portaudio_wrapper.h"
using namespace std;


namespace FFT
{
    /* The transformer provides an efficient framework for computing fixed size
     * FFTs. Once initialized, it precomputes lookup tables for bitreversal and
     * twiddle factors. You can then utilize these to perform FFTs.
     */
    class Transformer
    {
        private:
            const unsigned size;
            const unsigned buffer_size;

            vector<unsigned> bit_reverses;
            vector<complex<SAMPLE> > twiddles;

        public:
            Transformer(unsigned in_size);

            /* Given the input complex numbers, computes the fft and places the
             * result in the output buffer. May destructively modify the input
             * buffer. */
            void fft(complex<SAMPLE>* in, complex<SAMPLE>* out);
            void ifft(complex<SAMPLE>* in, complex<SAMPLE>* out);
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
