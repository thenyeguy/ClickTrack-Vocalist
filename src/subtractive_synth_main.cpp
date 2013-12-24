#include <iostream>
#include "../src/io_elements.h"
#include "../src/midi_wrapper.h"
#include "../src/subtractive_synth.h"

int main()
{
    using namespace std;

    cout << "Initializing MIDI instrument" << endl;
    Instruments::SubtractiveSynth inst(10,1);

    cout << "Initializing signal chain" << endl;
    FilterGenerics::OutputChannel* instOut = inst.get_output_channel();
    Filters::ClipDetector clip(1.0, &instOut);

    FilterGenerics::OutputChannel* clipOut = clip.get_output_channel();
    IOElements::Speaker out(&clipOut);

    cout << "Entering playback loop..." << endl;
    while(true)
    {
        out.consume_inputs();
    }

    
    return 0;
}
