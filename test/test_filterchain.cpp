#include <iostream>
#include "../src/io_elements.h"
#include "../src/oscillator.h"
#include "../src/elementary_filters.h"
#include "../src/delay.h"

using namespace std;
using namespace Portaudio;
using namespace IOElements;
using namespace Oscillators;
using namespace Filters;


OutputChannel** singleToList(OutputChannel* item)
{
    OutputChannel** result = new OutputChannel*[1];
    result[0] = item;
    return result;
}

int main()
{
    cout << "Initializing signal chain" << endl;
    Microphone mic;
    SimpleDelay delay(singleToList(mic.get_output_channel()));
    GainFilter mic_gain(0.5, singleToList(delay.get_output_channel()));

    TriangleWave tri(440.f);
    GainFilter tri_gain(0.5, singleToList(tri.get_output_channel()));

    OutputChannel* channels[2] =
        {mic_gain.get_output_channel(), tri_gain.get_output_channel()};
    Adder add(channels, 2);
    Speaker speaker(channels, 2);




    cout << "Entering process loop" << endl;
    while(true)
    {
        try
        {
            speaker.consume_inputs();
        }
        catch(exception& e)
        {
            cerr << "\n\n" << "EXCEPTION: " << e.what() << endl;
            exit(1);
        }
    }
}
