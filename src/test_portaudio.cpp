#include <iostream>
#include "portaudio_wrapper.h"
using namespace std;
using namespace Portaudio;


int main(int argc, char* argv[])
{
    InputStream* in = new InputStream();
    OutputStream* out = new OutputStream();
    float* buffer = new float[DEFAULT_BUFFER_SIZE];

    while(true)
    {
        in->readFromStream(buffer, DEFAULT_BUFFER_SIZE);
        out->writeToStream(buffer, DEFAULT_BUFFER_SIZE);
    }

    return 0;
}
