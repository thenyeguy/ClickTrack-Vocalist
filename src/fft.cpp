#include <iostream>
#include <ctime>
#include "fft.h"

using namespace ClickTrack;
const float PI = 3.14159265358979f;


FFTW::FFTW(unsigned in_size)
    : buffer_size(in_size)
{
    // Statically allocate buffers
    real_buffer = new double[buffer_size];
    complex_buffer = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * buffer_size);

    // Create the plans
    forward = fftw_plan_dft_r2c_1d(buffer_size, real_buffer, complex_buffer, 
            FFTW_ESTIMATE);
    backward = fftw_plan_dft_c2r_1d(buffer_size, complex_buffer, real_buffer, 
            FFTW_ESTIMATE);
}
FFTW::~FFTW()
{
    // Free the plans
    fftw_destroy_plan(forward);
    fftw_destroy_plan(backward);

    // Free the buffers
    delete[] real_buffer;
    fftw_free(complex_buffer);
}


void FFTW::fft(SAMPLE* in, std::complex<SAMPLE>* out)
{
    // Copy in from the input buffer
    for(unsigned i = 0; i < buffer_size; i++)
        real_buffer[i] = in[i];

    // Execute the fft
    fftw_execute(forward);

    // Copy into the output buffer
    for(unsigned i = 0; i < buffer_size; i++)
    {
        std::complex<double>* c =
            reinterpret_cast<std::complex<double>*>(&complex_buffer[i]);
        out[i] = *c;
    }
}


void FFTW::ifft(std::complex<SAMPLE>* in, SAMPLE* out)
{
    // Copy in from the input buffer
    std::complex<double>* shadow = 
        reinterpret_cast<std::complex<double>*>(in);
    for(unsigned i = 0; i < buffer_size; i++)
    {
        shadow[i] = in[i];
    }

    // Execute the fft
    fftw_execute(backward);

    // Copy into the output buffer
    for(unsigned i = 0; i < buffer_size; i++)
        out[i] = real_buffer[i] / buffer_size;
}




Transformer::Transformer(unsigned in_size)
    : size(in_size), buffer_size(next_power_of_two(in_size)),
      bit_reverses(), twiddles()
{
    // Populate the bit reverses
    // Uses bit shift because we only use the lower log2(size) bits to express
    // the index, and these get shoved into the upper bits of the reverse
    for(int i = 0; i < buffer_size; i++)
        bit_reverses.push_back(bit_reverse(i) >> (32-int_log(buffer_size)));

    // Populate the twiddles factors w_n^i
    // w_n = exp(j*2*pi*n/N)
    std::complex<SAMPLE> exponent(0,-2*PI/buffer_size);
    for(int i = 0; i < buffer_size; i++)
        twiddles.push_back(exp(exponent * std::complex<SAMPLE>(i,0)));
}


void Transformer::fft(std::complex<SAMPLE>* in, std::complex<SAMPLE>* out)
{
    // Copy the output into bit reverse order
    for(unsigned i = 0; i < buffer_size; i++)
    {
        unsigned reverse_i = bit_reverses[i];
        if(i > size)
            out[reverse_i] = std::complex<SAMPLE>(0,0);
        else
            out[reverse_i] = in[i];
    }

    // Iteratively perform FFT, starting at 2 poitns
    for(unsigned N = 2; N <= buffer_size; N *= 2)
    {
        // For each of the small FFTs
        for(unsigned set = 0; set < buffer_size/N; set++)
        {
            // For each pair of N
            for(unsigned i = 0; i < N/2; i++)
            {
                unsigned lower_i = N*set + i;
                unsigned upper_i = lower_i + N/2;

                // Grab out the lower and upper N
                std::complex<SAMPLE> lower = out[lower_i];
                std::complex<SAMPLE> upper =
                    out[upper_i] * twiddles[buffer_size/N * i];

                // Assign them back using a butterfly
                out[lower_i] = lower + upper;
                out[upper_i] = lower - upper;
            }
        }
    }
}


void Transformer::ifft(std::complex<SAMPLE>* in, std::complex<SAMPLE>* out)
{
    // conjugate the input
    for(int i = 0; i < size; i++)
    {
        in[i] = conj(in[i]);
    }

    // perform forward fft
    fft(in, out);

    // conjugate and normalize the output
    for(int i = 0; i < size; i++)
    {
        out[i] = conj(out[i]) / ((SAMPLE)buffer_size);
    }
}


unsigned ClickTrack::next_power_of_two(unsigned in)
{
    in--;
    in |= in >> 1;
    in |= in >> 2;
    in |= in >> 4;
    in |= in >> 8;
    in |= in >> 16;
    in++;

    return in;
}


unsigned ClickTrack::bit_reverse(unsigned in)
{
    in = (in >> 16) | (in << 16);
    in = ((in & 0xFF00FF00) >> 8) | ((in & 0x00FF00FF) << 8);
    in = ((in & 0xF0F0F0F0) >> 4) | ((in & 0x0F0F0F0F) << 4);
    in = ((in & 0xCCCCCCCC) >> 2) | ((in & 0x33333333) << 2);
    in = ((in & 0xAAAAAAAA) >> 1) | ((in & 0x55555555) << 1);
    return in;
}


unsigned ClickTrack::int_log(unsigned in)
{
    in--;
    unsigned result = 0;
    while(in > 0)
    {
        in = in >> 1;
        result++;
    }
    return result;
}
