#include "speaker.h"

using namespace ClickTrack;


Speaker::Speaker(unsigned num_inputs, bool defaultDevice)
    : AudioConsumer(num_inputs), buffer(), stream(num_inputs,defaultDevice),
      callback(NULL), payload(NULL)
{
    for(unsigned i = 0; i < num_inputs; i++)
        buffer.push_back(std::vector<SAMPLE>(BUFFER_SIZE));
}


void Speaker::process_inputs(std::vector<SAMPLE>& inputs, unsigned long t)
{
    // Copy one frame in
    for(unsigned i = 0; i < inputs.size(); i++)
        buffer[i][t % BUFFER_SIZE] = inputs[i];
    
    // If we have filled our buffer, write out
    if((t+1) % BUFFER_SIZE == 0)
    {
        stream.writeToStream(buffer);

        // Run the callback
        if(callback != NULL)
            callback(get_next_time()+1, payload);
    }
}


void Speaker::register_callback(callback_t in_callback, void* in_payload)
{
    callback = in_callback;
    payload = in_payload;
}
