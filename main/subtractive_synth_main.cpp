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
    Filters::ClipDetector clip(1.0);
    clip.set_input_channel(inst.get_output_channel());

    IOElements::Speaker out;
    out.set_input_channel(clip.get_output_channel());

    cout << "Entering playback loop..." << endl << endl;
    while(true)
    {
        out.consume_inputs();
    }

    
    return 0;
}
