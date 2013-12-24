#include <iostream>
#include "../src/io_elements.h"
#include "../src/midi_wrapper.h"
#include "../src/simple_midi_instrument.h"

int main()
{
    using namespace std;

    cout << "Initializing MIDI instrument" << endl;
    Instruments::SimpleMidiInstrument inst(1);

    cout << "Initializing signal chain" << endl;
    FilterGenerics::OutputChannel* instOut = inst.get_output_channel();
    IOElements::Speaker out(&instOut);

    cout << "Entering playback loop..." << endl;
    while(true)
    {
        out.consume_inputs();
    }

    
    return 0;
}
