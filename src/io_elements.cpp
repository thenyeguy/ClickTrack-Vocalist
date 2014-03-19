#include <iostream>
#include "io_elements.h"

using namespace ClickTrack;


Microphone::Microphone(unsigned num_channels, bool defaultDevice)
    : AudioGenerator(num_channels), buffer(),
      stream(num_channels,defaultDevice)
{
    for(unsigned i = 0; i < num_channels; i++)
        buffer.push_back(std::vector<SAMPLE>(PORTAUDIO_BUFFER_SIZE));
}


void Microphone::generate_outputs(std::vector<SAMPLE>& outputs, unsigned long t)
{
    // If we have run out of samples, refill our buffer
    if(t % PORTAUDIO_BUFFER_SIZE == 0)
        stream.readFromStream(buffer);

    // Copy one frame out
    for(unsigned i = 0; i < outputs.size(); i++)
        outputs[i] = buffer[i][t % PORTAUDIO_BUFFER_SIZE];
}



Speaker::Speaker(unsigned num_inputs, bool defaultDevice)
    : AudioConsumer(num_inputs), buffer(),
      stream(num_inputs,defaultDevice)
{
    for(unsigned i = 0; i < num_inputs; i++)
        buffer.push_back(std::vector<SAMPLE>(PORTAUDIO_BUFFER_SIZE));
}


void Speaker::process_inputs(std::vector<SAMPLE>& inputs, unsigned long t)
{
    // Copy one frame in
    for(unsigned i = 0; i < inputs.size(); i++)
        buffer[i][t % PORTAUDIO_BUFFER_SIZE] = inputs[i];
    
    // If we have filled our buffer, write out
    if((t+1) % PORTAUDIO_BUFFER_SIZE == 0)
        stream.writeToStream(buffer);
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
        throw InvalidWavFile("No support for sample rates other than 44100");

    file.read(container.raw, 4); // toss byte rate

    // Read alignment
    file.read(container.raw, 4);
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


bool WavReader::is_done()
{
    return samples_read= samples_total;
}


void WavReader::restart()
{
    samples_read = 0;
    file.seekg(44); // move to end of headers
}


unsigned WavReader::get_total_samples()
{
    return samples_total;
}


void WavReader::generate_outputs(std::vector<SAMPLE>& outputs, unsigned long t)
{
    // TODO: support more than 16-bit
    union {
        char raw[2];
        signed short val;
    } left, right;
    left.val = 0; right.val = 0;

    // Silence at end
    if(samples_read == samples_total)
    {
        outputs[0] = 0;
        outputs[1] = 0;
        return;
    }

    // If we have stereo audio, read right channel
    // Otherwise copy left channel
    file.read(left.raw, byte_depth);
    if(stereo)
        file.read(right.raw, byte_depth);
    else
        right.val = left.val;

    outputs[0] = ((SAMPLE)left.val) / 32768;
    outputs[1] = ((SAMPLE)right.val) / 32768;

    samples_read++;
}



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
