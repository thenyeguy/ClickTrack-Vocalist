#ifndef FFT_H
#define FFT_H

#include <complex>
#include <vector>
#include <fftw3.h>
#include "portaudio_wrapper.h"


namespace ClickTrack
{
    /* The FFTW object wraps the FFTW3 library to provide a fast FFT for use in
     * processing.
     *
     * Only performs transforms where the time domain signal is real
     */
    class FFTW
    {
        public:
            FFTW(unsigned in_size);
            ~FFTW();

            /* Given the input complex numbers, computes the fft and places the
             * result in the output buffer. May destructively modify the input
             * buffer. */
            void fft(SAMPLE* in, std::complex<SAMPLE>* out);
            void ifft(std::complex<SAMPLE>* in, SAMPLE* out);

        private:
            /* These "plans" represent the state used by the FFTW library to
             * perform actual transforms. They are created at initialization
             * time and passed to the runtime functions to perform transforms
             */
            fftw_plan forward;
            fftw_plan backward;

            /* Statically allocate these buffers for speed
             */
            const unsigned buffer_size;
            double* real_buffer;
            fftw_complex* complex_buffer;
    };

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
