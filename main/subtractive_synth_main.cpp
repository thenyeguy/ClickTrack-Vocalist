#include <iostream>
#include "../src/clip_detector.h"
#include "../src/limiter.h"
#include "../src/midi_wrapper.h"
#include "../src/speaker.h"
#include "../src/subtractive_synth.h"

int main()
{
    using namespace std;

    cout << "Initializing MIDI instrument" << endl;
    SubtractiveSynth inst(10);
    MidiListener midi(&inst, 1);

    inst.filter.set_cutoff(1000);
    inst.set_lfo_vibrato(0.1);
    inst.set_lfo_tremelo(.2);

    cout << "Initializing signal chain" << endl;
    Limiter limiter(-3.0);
    limiter.set_input_channel(inst.get_output_channel());

    ClipDetector clip(1.0);
    clip.set_input_channel(limiter.get_output_channel());

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
