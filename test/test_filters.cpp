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


OutputChannel** singleToList(OutputChannel* item)
{
    OutputChannel** result = new OutputChannel*[1];
    result[0] = item;
    return result;
}

int main()
{
    cout << "Initializing signal chain" << endl;
    SawWave saw(440.f);
    GainFilter saw_gain(0.7, singleToList(saw.get_output_channel()));

    ShelfFilter shelf(ShelfFilter::low, 1000, -10,
        singleToList(saw_gain.get_output_channel()), 1);

    Speaker speaker(singleToList(shelf.get_output_channel()), 1);




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
