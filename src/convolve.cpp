#include <iostream>
#include <ctime>
#include <complex>
#include "convolve.h"

using namespace FilterGenerics;
using namespace Filters;


ConvolutionFilter::ConvolutionFilter(OutputChannel* in_input_channel,
                                     unsigned impulse_length,
                                     SAMPLE* in_impulse_response)
    : AudioFilter(1, 1, &in_input_channel),

      transform_size(8*DEFAULT_BLOCK_SIZE),
      transformer(transform_size),

      num_impulse_blocks((impulse_length - 1) / 
                         (transform_size-DEFAULT_BLOCK_SIZE) + 1),
        // shift by one so perfect powers of two don't get overallocated
      impulse_response(num_impulse_blocks, NULL),

      reverb_buffer(num_impulse_blocks * transform_size)
{
    // Preallocate the buffers
    input_buffer = new complex<SAMPLE>[transform_size];
    middle_buffer = new complex<SAMPLE>[transform_size];
    output_buffer = new complex<SAMPLE>[transform_size];

    // Zero the input buffer
    for(int i=0; i < transform_size; i++)
        input_buffer[i] = 0.0;


    // Compute impulse energy to normalize
    float energy = 0.0f;
    for(int i=0; i < impulse_length; i++)
        energy += pow(in_impulse_response[i], 2);
    energy = sqrt(energy);


    // Split the impulse response into segments
    for(int i=0; i < num_impulse_blocks; i++)
    {
        // For each segment, take its FFT
        for(int j=0; j < transform_size-DEFAULT_BLOCK_SIZE; j++)
        {
            int t = (transform_size-DEFAULT_BLOCK_SIZE)*i + j;
            if(t < impulse_length)
                input_buffer[j] = in_impulse_response[t] / energy;
            else
                input_buffer[j] = 0.0;
        }

        impulse_response[i] = new complex<SAMPLE>[transform_size];
        transformer.fft(input_buffer, impulse_response[i]);
    }


    // Rezero the input buffer
    for(int i=0; i < transform_size; i++)
        input_buffer[i] = 0.0;


    // Initialize the output buffers
    for(int i=0; i < num_impulse_blocks * transform_size; i++)
    {
        reverb_buffer.add(0);
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
    // First take the FFT of the input signal
    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
        input_buffer[i] = input[0][i];
    transformer.fft(input_buffer, middle_buffer);


    // Then perform each overlap add step by...
    for(int i=0; i < num_impulse_blocks; i++)
    {
        // Frequency multiply
        for(int j=0; j < transform_size; j++)
            input_buffer[j] = middle_buffer[j] * impulse_response[i][j];

        //  Inverse transform
        transformer.ifft(input_buffer, output_buffer);
        for(int j=0; j < transform_size; j++)
            reverb_buffer[next_t + DEFAULT_BLOCK_SIZE*i + j] += 
                output_buffer[j].real();
    }


    // Extract the new output set
    for(int i=0; i < DEFAULT_BLOCK_SIZE; i++)
        output[0][i] = reverb_buffer[next_t + i];


    // Push the next set of empty values to buffer
    for(int i=0; i < DEFAULT_BLOCK_SIZE; i++)
        reverb_buffer.add(0.0);
}
