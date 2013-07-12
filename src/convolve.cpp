#include <iostream>
#include <complex>
#include "convolve.h"

using namespace FilterGenerics;
using namespace Filters;


// We will use portions of the input and impulse response of size
// DEFAULT_BLOCK_SIZE/2, so that their individual convolution is
// not larger than DEFAULT_BLOCK_SIZE. This allows us to IFFT
// one block size at a time for returns
ConvolutionFilter::ConvolutionFilter(OutputChannel* in_input_channel,
                                     unsigned impulse_length,
                                     SAMPLE* in_impulse_response)
    : AudioFilter(1, 1, &in_input_channel),
      transformer(DEFAULT_BLOCK_SIZE),
      output_length(DEFAULT_BLOCK_SIZE + impulse_length - 1),
      overlaps(output_length+DEFAULT_BLOCK_SIZE),
      num_impulse_blocks(2*((impulse_length - 1)/DEFAULT_BLOCK_SIZE + 1)),
        // shift by one so perfect powers of two don't get overallocated
      impulse_response(num_impulse_blocks, NULL)
{
    // Preallocate the input buffer for speed
    input_buffer = new complex<SAMPLE>[DEFAULT_BLOCK_SIZE];
    output_buffer = new complex<SAMPLE>[DEFAULT_BLOCK_SIZE];

    // Zero the input buffer to use it shortly
    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
        input_buffer[i] = 0.0f;

    // Split the impulse into blocks and find their FFTs
    for(int i = 0; i < num_impulse_blocks; i++)
    {
        // Get one block of samples
        for(int j = 0; j < DEFAULT_BLOCK_SIZE/2; j++)
        {
            if(j == num_impulse_blocks - 1)
                input_buffer[j] = 0;
            else
                input_buffer[j] =
                    in_impulse_response[i*DEFAULT_BLOCK_SIZE/2 + j];
        }

        // FFT and store it
        complex<SAMPLE>* out = new complex<SAMPLE>[DEFAULT_BLOCK_SIZE];
        transformer.fft(input_buffer, out);
        impulse_response[i] = out;
    }

    // Fill the overlap buffer with 0s for the -1th block of samples
    for(int i = 0; i < output_length; i++)
        overlaps.add(0.0);
}


ConvolutionFilter::~ConvolutionFilter()
{
    for(int i = 0; i < num_impulse_blocks; i++)
        delete impulse_response[i];
    delete input_buffer;
    delete output_buffer;
}


void ConvolutionFilter::filter(SAMPLE** input, SAMPLE** output)
{
    // First add space for the last block of samples in the buffer
    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
        overlaps.add(0);

    // Populate the input buffer for half one
    for(int i = 0; i < DEFAULT_BLOCK_SIZE/2; i++)
        input_buffer[i] = complex<SAMPLE>(input[0][i]);
    for(int i = DEFAULT_BLOCK_SIZE/2; i < DEFAULT_BLOCK_SIZE; i++)
        input_buffer[i] = complex<SAMPLE>(0,0);

    // Grab its FFT
    transformer.fft(input_buffer, output_buffer);

    // For each block in the impulse response, we multiply in the
    // frequency domain, then add it to the output buffer
    for(int i = 0; i < num_impulse_blocks; i++)
    {
        for(int j = 0; j < DEFAULT_BLOCK_SIZE/2; j++)
        {
            overlaps[next_t + i*DEFAULT_BLOCK_SIZE + j] =
                output_buffer[j] * impulse_response[i][j];
        }
    }

    // Grab the next block of outputs from the buffer
    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
        input_buffer[i] = overlaps[next_t + i];

    transformer.fft(input_buffer, output_buffer);

    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
        output[0][i] = output_buffer[i].real();
}

/*
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
*/
