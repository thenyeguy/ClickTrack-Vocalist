#include <iostream>
#include "ioelements.h"
#include "delay.h"

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
    SimpleDelay del(1, &mic.get_output_channel(0));

    cout << "Initializing speaker" << endl;
    Speaker speak(out, &del.get_output_channel(0));

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
