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

      num_impulse_blocks((impulse_length - 1)/(DEFAULT_BLOCK_SIZE/2) + 1),
        // shift by one so perfect powers of two don't get overallocated
      impulse_response(num_impulse_blocks, NULL),

      left_buffer(num_impulse_blocks * DEFAULT_BLOCK_SIZE),
      right_buffer(num_impulse_blocks * DEFAULT_BLOCK_SIZE),
      output_overflow(DEFAULT_BLOCK_SIZE/2, 0.0f)
{
    // Preallocate the buffers
    input_buffer = new complex<SAMPLE>[DEFAULT_BLOCK_SIZE];
    output_buffer = new complex<SAMPLE>[DEFAULT_BLOCK_SIZE];

    left_out_buffer = new complex<SAMPLE>[DEFAULT_BLOCK_SIZE];
    right_out_buffer = new complex<SAMPLE>[DEFAULT_BLOCK_SIZE];

    // Zero the input buffers
    for(int i=0; i < DEFAULT_BLOCK_SIZE; i++)
    {
        input_buffer[i] = 0.0;
        output_buffer[i] = 0.0;
        left_out_buffer[i] = 0.0;
        right_out_buffer[i] = 0.0;
    }


    // Compute impulse energy to normalize
    float energy = 0.0f;
    for(int i=0; i < impulse_length; i++)
        energy += pow(in_impulse_response[i], 2);
    energy = sqrt(energy);


    // Split the impulse response into segments
    for(int i=0; i < num_impulse_blocks; i++)
    {
        // For each segment, take its FFT
        for(int j=0; j < DEFAULT_BLOCK_SIZE/2; j++)
            input_buffer[j] = in_impulse_response[DEFAULT_BLOCK_SIZE/2*i + j];
        transformer.fft(input_buffer, output_buffer);

        // Copy it into the impulse array
        impulse_response[i] = new complex<SAMPLE>[DEFAULT_BLOCK_SIZE];
        for(int j=0; j < DEFAULT_BLOCK_SIZE; j++)
            impulse_response[i][j] = output_buffer[j];
    }


    // Initialize the output buffers
    for(int i=0; i < DEFAULT_BLOCK_SIZE * num_impulse_blocks; i++)
    {
        left_buffer.add(0);
        right_buffer.add(0);
    }
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
    // First take the FFT of the input signal's halves
    for(int i = 0; i < DEFAULT_BLOCK_SIZE/2; i++)
    {
        input_buffer[i] = input[0][i];
        transformer.fft(input_buffer, left_out_buffer);

        input_buffer[i] = input[0][i+DEFAULT_BLOCK_SIZE/2];
        transformer.fft(input_buffer, right_out_buffer);
    }

    // Then perform the frequency multiplications
    for(int i=0; i < num_impulse_blocks; i++)
    {
        for(int j=0; j < DEFAULT_BLOCK_SIZE; j++)
        {
            left_buffer[next_t + j] += left_out_buffer[j] * 
                impulse_response[i][j];
            right_buffer[next_t + j] += right_out_buffer[j] * 
                impulse_response[i][j];
        }
    }


    // First add the last block's overflow
    for(int i=0; i < DEFAULT_BLOCK_SIZE/2; i++)
        output[0][i] = output_overflow[i];

    // Grab the next output set
    for(int i=0; i < DEFAULT_BLOCK_SIZE; i++)
    {
        left_out_buffer[i] = left_buffer[next_t + i];
        right_out_buffer[i] = right_buffer[next_t + i];
    }
    
    // Reverse the left half
    transformer.ifft(left_out_buffer, output_buffer);
    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
        output[0][i] = output_buffer[i].real();

    // Reverse the right half
    transformer.ifft(right_out_buffer, output_buffer);
    for(int i = 0; i < DEFAULT_BLOCK_SIZE/2; i++)
        output[0][i+DEFAULT_BLOCK_SIZE/2] += output_buffer[i].real();
    for(int i = DEFAULT_BLOCK_SIZE/2; i < DEFAULT_BLOCK_SIZE; i++)
        output_overflow[i - DEFAULT_BLOCK_SIZE/2] = output_buffer[i].real();


    // Push the next set of empty values to buffer
    for(int i=0; i < DEFAULT_BLOCK_SIZE; i++)
    {
        left_buffer.add(0.0);
        right_buffer.add(0.0);
    }
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
