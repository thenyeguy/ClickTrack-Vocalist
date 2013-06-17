#include <iostream>
#include "ioelements.h"

using namespace std;
using namespace Portaudio;
using namespace IOElements;

int main()
{
    cout << "Initializing IO streams" << endl;
    InputStream* in = new InputStream();
    OutputStream* out = new OutputStream();

    cout << "Initializing microphone" << endl;
    Microphone mic(in);

    cout << "Initializing speaker" << endl;
    Speaker speak(out, &mic.get_output_channel(0));

    cout << "Entering process loop" << endl;
    while(true)
    {
        speak.consume_inputs();
    }
}
