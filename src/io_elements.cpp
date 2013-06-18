#include <iostream>
#include "io_elements.h"

using namespace FilterGenerics;
using namespace IOElements;


Microphone::Microphone(Portaudio::InputStream* inputs, unsigned num_channels)
    : AudioGenerator(num_channels)
{
    streams = inputs;
}


void Microphone::generate_outputs(SAMPLE** outputs)
{
    for(int i = 0; i < num_output_channels; i++)
        streams[i].readFromStream(outputs[i], DEFAULT_BLOCK_SIZE);
}



Speaker::Speaker(Portaudio::OutputStream* outputs,
                 FilterGenerics::OutputChannel** inputs,
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
