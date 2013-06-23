#include <iostream>
#include <complex>
#include "convolve.h"

using namespace FilterGenerics;
using namespace Filters;


ConvolutionFilter::ConvolutionFilter(OutputChannel* in_input_channel,
                                     unsigned impulse_length,
                                     SAMPLE* in_impulse_response)
    : AudioFilter(1, 1, &in_input_channel),
      output_length(FFT::next_power_of_two(DEFAULT_BLOCK_SIZE + impulse_length -1)),
      overlaps(output_length+DEFAULT_BLOCK_SIZE), transformer(output_length)
{
    // Populate the input array to take the FFT of the impulse
    complex<SAMPLE>* in_impulse_complex = new complex<SAMPLE>[output_length];
    for(int i = 0; i < impulse_length; i++)
        in_impulse_complex[i] = complex<SAMPLE>(in_impulse_response[i]);
    for(int i = impulse_length; i < output_length; i++)
        in_impulse_complex[i] = complex<SAMPLE>(0,0);

    // Get the FFT of the impulse
    impulse_response = new complex<SAMPLE>[output_length];
    transformer.fft(in_impulse_complex, impulse_response);
    delete in_impulse_complex;

    // Preallocate the input buffer for speed
    input_buffer = new complex<SAMPLE>[output_length];
    output_buffer = new complex<SAMPLE>[output_length];

    // Fill the overlap buffer with 0s
    for(int i = 0; i < output_length; i++)
        overlaps.add(0.0);
}


ConvolutionFilter::~ConvolutionFilter()
{
    delete impulse_response;
    delete input_buffer;
    delete output_buffer;
}


void ConvolutionFilter::filter(SAMPLE** input, SAMPLE** output)
{
    // Populate the input buffer
    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
        input_buffer[i] = complex<SAMPLE>(input[0][i]);
    for(int i = DEFAULT_BLOCK_SIZE; i < output_length; i++)
        input_buffer[i] = complex<SAMPLE>(0,0);

    // FFT then convolve
    transformer.fft(input_buffer, output_buffer);

    for(int i = 0; i < output_length; i++)
        input_buffer[i] = output_buffer[i] * impulse_response[i];
    transformer.ifft(input_buffer, output_buffer);

    // Write the result into the output buffer
    // Add in the overlapping portions
    for(int i = 0; i < output_length; i++)
        overlaps[next_t + i] += std::abs(output_buffer[i]);

    // Enqueue the new results
    for(int i = output_length - DEFAULT_BLOCK_SIZE; i < output_length; i++)
        overlaps.add(std::abs(output_buffer[i]));

    // Grab the next block of outputs from the buffer
    overlaps.get_range(output[0], next_t, next_t+DEFAULT_BLOCK_SIZE);
}
