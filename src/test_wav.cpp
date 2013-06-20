#include <iostream>
#include "filter_generics.h"
#include "io_elements.h"
#include "oscillator.h"

using namespace std;
using namespace Portaudio;
using namespace IOElements;
using namespace Oscillators;



int main()
{
    try
    {
        cout << "Initializing signal chain" << endl;
        WavReader wav("test.wav");
        TriangleWave tri(440);
        SinWave sin(880);
        FilterGenerics::OutputChannel* chans[2] =
            {wav.get_output_channel(0), sin.get_output_channel(1)};
        Speaker speaker(chans, 1);

        cout << "Entering process loop" << endl;
        while(true)
            speaker.consume_inputs();
    }
    catch(exception& e)
    {
        cerr << "\n\n" << "EXCEPTION: " << e.what() << endl;
        exit(1);
    }
}
