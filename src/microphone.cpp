#include "microphone.h"

using namespace ClickTrack;


Microphone::Microphone(unsigned num_channels, bool defaultDevice)
    : AudioGenerator(num_channels), buffer(),
      stream(num_channels,defaultDevice)
{
    for(unsigned i = 0; i < num_channels; i++)
        buffer.push_back(std::vector<SAMPLE>(BUFFER_SIZE));
}


void Microphone::generate_outputs(std::vector<SAMPLE>& outputs, unsigned long t)
{
    // If we have run out of samples, refill our buffer
    if(t % BUFFER_SIZE == 0)
        stream.readFromStream(buffer);

    // Copy one frame out
    for(unsigned i = 0; i < outputs.size(); i++)
        outputs[i] = buffer[i][t % BUFFER_SIZE];
}
