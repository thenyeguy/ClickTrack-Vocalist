#include <iostream>
#include "io_elements.h"

using namespace FilterGenerics;
using namespace IOElements;


Microphone::Microphone(unsigned num_channels)
    : AudioGenerator(num_channels),
      stream(num_channels)
{}


void Microphone::generate_outputs(SAMPLE** outputs)
{
    for(int i = 0; i < num_output_channels; i++)
        stream.readFromStream(outputs[i], DEFAULT_BLOCK_SIZE);
}



Speaker::Speaker(FilterGenerics::OutputChannel** inputs,
                 unsigned num_inputs)
    : AudioConsumer(num_inputs, inputs),
      stream(num_inputs)
{}


void Speaker::process_inputs(SAMPLE** inputs)
{
    // Interleave channels
    unsigned num_samples = num_input_channels * DEFAULT_BLOCK_SIZE;
    SAMPLE buffer[num_samples];

    for(int i = 0; i < num_input_channels; i++)
    {
        for(int j = 0; j < DEFAULT_BLOCK_SIZE; j++)
            buffer[num_input_channels*j + i] = inputs[i][j];
    }

    // Write out
    stream.writeToStream(buffer, DEFAULT_BLOCK_SIZE);
}



WavReader::WavReader(const char* in_filename)
    : AudioGenerator(2), filename(in_filename) // always stereo
{
    // Set up file to read
    file.open(filename, std::ios::in|std::ios::binary);
    union {
        char raw[4];
        unsigned short two[2];
        unsigned int four;
    } container;


    // Parse RIFF HEADER
    file.read(container.raw, 4);
    if(container.four != 0x46464952)
        throw InvalidWavFile("No RIFF identifier");

    file.read(container.raw, 4); // toss size

    file.read(container.raw, 4);
    if(container.four != 0x45564157)
        throw InvalidWavFile("No WAV identifier");


    // Parse fmt header
    file.read(container.raw, 4);
    if(container.four != 0x20746d66)
        throw InvalidWavFile("No fmt section");

    // Parse size
    file.read(container.raw, 4);
    if(container.four != 16)
        throw InvalidWavFile("Invalid fmt length");

    // Read format and channel count
    file.read(container.raw, 4);
    if(container.two[0] != 1)
        throw InvalidWavFile("No support for nonlinear quantization");
    stereo = container.two[1] == 2;

    // Read sample rate
    file.read(container.raw, 4);
    if(container.four != 44100)
        throw InvalidWavFile("No support for nonlinear quantization");

    file.read(container.raw, 4); // toss byte rate

    // Read alignment
    file.read(container.raw, 4); // toss byte rate
    byte_depth = container.two[1]/8;
    if(byte_depth != 2)
        throw InvalidWavFile("Unsupported byte depth");


    // Parse data header
    file.read(container.raw, 4);
    if(container.four != 0x61746164)
        throw InvalidWavFile("No data section");

    // Parse size
    file.read(container.raw, 4);
    samples_total = container.four / byte_depth;
    if(stereo) samples_total /= 2;

    // Start with nothing played
    samples_read = 0;
}


void WavReader::generate_outputs(SAMPLE** outputs)
{
    // TODO: support more than 16-bit
    union {
        char raw[2];
        signed short val;
    } left, right;
    left.val = 0; right.val = 0;

    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
    {
        // Silence at end
        if(samples_read >= samples_total)
        {
            outputs[0][i] = 0;
            outputs[1][i] = 0;
            continue;
        }

        // If we have stereo audio, read right channel
        // Otherwise copy left channel
        file.read(left.raw, byte_depth);
        if(stereo)
            file.read(right.raw, byte_depth);
        else
            right.val = left.val;

        outputs[0][i] = ((SAMPLE)left.val) / 32768;
        outputs[1][i] = ((SAMPLE)right.val) / 32768;

        samples_read++;
    }
}
