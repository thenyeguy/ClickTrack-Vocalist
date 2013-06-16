#include <iostream>
#include "ioblocks.h"


using namespace FilterChain;


Microphone::Microphone(Portaudio::InputStream* inputs, unsigned num_channels)
    : AudioElement(num_channels)
{
    streams = inputs;
}


void Microphone::generate_output()
{
    SAMPLE buffer[DEFAULT_BLOCK_SIZE];
    for(int i = 0; i < num_output_channels; i++)
    {
        //Read one stream and copy to its output buffer
        streams[i].readFromStream(buffer, DEFAULT_BLOCK_SIZE);

        for(int j = 0; j < DEFAULT_BLOCK_SIZE; j++)
            out_buffers[i].add_sample(buffer[j]);
    }
}


Speaker::Speaker(Portaudio::OutputStream* outputs,
                 AudioElement* input_elements, unsigned* input_chans,
                 unsigned num_input_elements)
    : OutputElement(input_elements, input_chans, num_input_elements)
{
    streams = outputs;
}


void Speaker::output(SAMPLE** inputs)
{
    for(int i = 0; i < num_inputs; i++)
        streams[i].writeToStream(inputs[i], DEFAULT_BLOCK_SIZE);
}
