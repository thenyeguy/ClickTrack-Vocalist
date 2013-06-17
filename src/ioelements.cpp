#include <iostream>
#include "ioelements.h"

using namespace FilterGenerics;
using namespace IOElements;


Microphone::Microphone(Portaudio::InputStream* inputs, unsigned num_channels)
    : AudioGenerator(num_channels)
{
    streams = inputs;
}


void Microphone::generate_outputs()
{
    SAMPLE buffer[DEFAULT_BLOCK_SIZE];
    for(int i = 0; i < num_output_channels; i++)
    {
        //Read one stream and copy to its output buffer
        streams[i].readFromStream(buffer, DEFAULT_BLOCK_SIZE);
        output_channels[i].fill_internal_buffer(buffer);
    }
}



Speaker::Speaker(Portaudio::OutputStream* outputs,
                 FilterGenerics::OutputChannel* inputs,
                 unsigned num_inputs)
    : AudioConsumer(num_inputs, inputs)
{
    streams = outputs;
}


void Speaker::process_inputs(SAMPLE** inputs)
{
    for(int i = 0; i < num_input_channels; i++)
        streams[i].writeToStream(inputs[i], DEFAULT_BLOCK_SIZE);
}
