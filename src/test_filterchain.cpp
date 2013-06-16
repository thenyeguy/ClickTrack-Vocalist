#include <iostream>
#include "ioblocks.h"

using namespace Portaudio;
using namespace FilterChain;

int main()
{
    InputStream* in = new InputStream();
    OutputStream* out = new OutputStream();

    Microphone mic(in);

    unsigned channels[] = {1};
    Speaker speak(out, &mic, channels, 1);

    while(true)
    {
        speak.process_inputs();
    }
}
