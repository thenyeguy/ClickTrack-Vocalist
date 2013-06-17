#include <iostream>
#include "ioelements.h"
#include "oscillator.h"
#include "unity.h"
#include "delay.h"

using namespace std;
using namespace Portaudio;
using namespace IOElements;
using namespace Oscillators;
using namespace Filters;

int main()
{
    cout << "Initializing IO streams" << endl;
    InputStream* in = new InputStream();
    OutputStream* out = new OutputStream();

    cout << "Initializing signal chain" << endl;
    Microphone mic(in);
    SimpleDelay delay(1, &mic.get_output_channel(0));
    UnityFilter unity(1, &delay.get_output_channel(0));
    Speaker speak(out, &unity.get_output_channel(0));

    SquareWave sq(440.0f);
    SinWave si(440.0f);
    TriangleWave tri(440.f);
    Speaker amp(out, &si.get_output_channel(0));

    cout << "Entering process loop" << endl;
    while(true)
    {
        try
        {
            amp.consume_inputs();
        }
        catch(exception& e)
        {
            cerr << "\n\n" << "EXCEPTION: " << e.what() << endl;
            exit(1);
        }
    }
}
