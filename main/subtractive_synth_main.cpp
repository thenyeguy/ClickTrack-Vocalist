#include <iostream>
#include "../src/io_elements.h"
#include "../src/midi_wrapper.h"
#include "../src/subtractive_synth.h"

int main()
{
    using namespace std;

    cout << "Initializing MIDI instrument" << endl;
    SubtractiveSynth inst(10);
    MidiListener midi(&inst, 1);

    cout << "Initializing signal chain" << endl;
    ClipDetector clip(1.0);
    clip.set_input_channel(inst.get_output_channel());

    Speaker out;
    out.set_input_channel(clip.get_output_channel());
    out.register_callback(MidiListener::timing_callback, &midi);

    cout << "Entering playback loop..." << endl << endl;
    while(true)
    {
        out.consume();
    }

    
    return 0;
}
