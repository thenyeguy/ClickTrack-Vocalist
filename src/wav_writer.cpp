#include <iostream>
#include "wav_writer.h"

using namespace ClickTrack;


WavWriter::WavWriter(const char* in_filename, unsigned num_inputs)
    : AudioConsumer(num_inputs), filename(in_filename)
{
    // Set up file to write
    file.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);


    // Write RIFF header
    file.write("RIFF", 4);
    file.write("\0\0\0\0",4); // dummy size field
    file.write("WAVE",4);

    // Write WAV header
    file.write("fmt ", 4);
    int section2size = 16;
    file.write((char*) &section2size, 4);
    short format = 1;
    file.write((char*) &format, 2);
    file.write((char*) &num_inputs, 2);
    unsigned sample_rate = SAMPLE_RATE;
    file.write((char*) &sample_rate, 4);
    unsigned byte_rate = sample_rate*num_inputs*16/8;
    file.write((char*) &byte_rate, 4);
    unsigned short block_align = num_inputs*16/8;
    file.write((char*) &block_align, 2);
    unsigned short bit_depth = 16;
    file.write((char*) &bit_depth, 2);

    // Write data header
    file.write("data", 4);
    file.write("\0\0\0\0", 4); // dummy size field
}


WavWriter::~WavWriter()
{
    unsigned data_size = samples_written*get_num_input_channels()*16/8;
    unsigned file_size = 36+data_size;
    file.seekp(4); file.write((char*) &file_size, 4);
    file.seekp(40); file.write((char*) &data_size, 4);

    file.close();
}


void WavWriter::process_inputs(std::vector<SAMPLE>& inputs, unsigned long t)
{
    for(int i = 0; i < inputs.size(); i++)
    {
        // Clip instead of overflowing
        SAMPLE sample = inputs[i];
        if(sample > 0.999f)  sample = 0.999f;
        if(sample < -0.999f) sample = -0.999f;

        signed short quantized = sample * 32768;
        file.write((char*) &quantized, 2);
    }
    samples_written++;
}
