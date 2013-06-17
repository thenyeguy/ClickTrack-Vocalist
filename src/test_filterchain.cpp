#include <iostream>
#include "ioelements.h"
#include "unity.h"

using namespace std;
using namespace Portaudio;
using namespace IOElements;
using namespace Filters;

int main()
{
    cout << "Initializing IO streams" << endl;
    InputStream* in = new InputStream();
    OutputStream* out = new OutputStream();

    cout << "Initializing microphone" << endl;
    Microphone mic(in);

    cout << "Initializing delay" << endl;
    UnityFilter unity(1, &mic.get_output_channel(0));

    cout << "Initializing speaker" << endl;
    Speaker speak(out, &unity.get_output_channel(0));

    cout << "Entering process loop" << endl;
    while(true)
    {
        try
        {
            speak.consume_inputs();
        }
        catch(exception& e)
        {
            cerr << "\n\n" << "EXCEPTION: " << e.what() << endl;
            exit(1);
        }
    }
}
