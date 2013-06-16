#include <iostream>
#include "portaudio_wrapper.h"
using namespace std;
using namespace Portaudio;


int main(int argc, char* argv[])
{
    cout << "Hello World!\n";

    Stream* pa = new Stream();
    float* buffer = new float[DEFAULT_BUFFER_SIZE];

    while(true)
    {
        pa->readFromStream(buffer, DEFAULT_BUFFER_SIZE);
        pa->writeToStream(buffer, DEFAULT_BUFFER_SIZE);
    }

    return 0;
}
