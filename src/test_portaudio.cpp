#include <iostream>
#include "portaudio_wrapper.h"
using namespace std;


int main(int argc, char* argv[])
{
    cout << "Hello World!\n";

    Portaudio::Stream* pa = new Portaudio::Stream();
    float* buffer = new float[PA_DEFAULT_BUFFER_SIZE];

    while(true)
    {
        pa->readFromStream(buffer, PA_DEFAULT_BUFFER_SIZE);
        pa->writeToStream(buffer, PA_DEFAULT_BUFFER_SIZE);
    }

    return 0;
}
