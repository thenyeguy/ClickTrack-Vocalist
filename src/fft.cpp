#include <iostream>
#include "fft.h"

using namespace std;
using namespace FFT;
const float PI = 3.14159265358979f;


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
    complex<SAMPLE> exponent(0,2*PI/buffer_size);
    for(int i = 0; i < buffer_size; i++)
        twiddles.push_back(exp(exponent * complex<SAMPLE>(i,0)));
}


void Transformer::fft(complex<SAMPLE>* in, complex<SAMPLE>* out)
{
    // Copy the output into bit reverse order
    for(unsigned i = 0; i < buffer_size; i++)
    {
        unsigned reverse_i = bit_reverses[i];
        if(i > size)
            out[reverse_i] = complex<SAMPLE>(0,0);
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
                complex<SAMPLE> lower = out[lower_i];
                complex<SAMPLE> upper =
                    out[upper_i] * twiddles[buffer_size/N * i];

                // Assign them back using a butterfly
                out[lower_i] = lower + upper;
                out[upper_i] = lower - upper;
            }
        }
    }
}


void Transformer::ifft(complex<SAMPLE>* in, complex<SAMPLE>* out)
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


unsigned FFT::next_power_of_two(unsigned in)
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


unsigned FFT::bit_reverse(unsigned in)
{
    in = (in >> 16) | (in << 16);
    in = ((in & 0xFF00FF00) >> 8) | ((in & 0x00FF00FF) << 8);
    in = ((in & 0xF0F0F0F0) >> 4) | ((in & 0x0F0F0F0F) << 4);
    in = ((in & 0xCCCCCCCC) >> 2) | ((in & 0x33333333) << 2);
    in = ((in & 0xAAAAAAAA) >> 1) | ((in & 0x55555555) << 1);
    return in;
}


unsigned FFT::int_log(unsigned in)
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
