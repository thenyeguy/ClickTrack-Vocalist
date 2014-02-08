#include <iostream>
#include "../src/io_elements.h"
#include "../src/oscillator.h"
#include "../src/elementary_filters.h"
#include "../src/filters.h"

using namespace std;
using namespace Portaudio;
using namespace IOElements;
using namespace Oscillators;
using namespace Filters;


int main()
{
    cout << "Initializing signal chain" << endl;
    SawWave saw(440.f);
    GainFilter saw_gain(0.7);
    saw_gain.set_input_channel(saw.get_output_channel());

    PassFilter pass(PassFilter::low, 5000);
    pass.set_input_channel(saw_gain.get_output_channel());

    Speaker speaker;
    speaker.set_input_channel(pass.get_output_channel());


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
