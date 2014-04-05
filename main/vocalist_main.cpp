#include <iostream>
#include "../src/clip_detector.h"
#include "../src/midi_wrapper.h"
#include "../src/vocalist.h"
#include "../src/speaker.h"


int main()
{
    using namespace std;

    cout << "Initializing MIDI instrument" << endl;
    Vocalist voice;
    MidiListener midi(&voice, 1);

    cout << "Creating signal chain" << endl;
    ClipDetector clip(1.0);
    clip.set_input_channel(voice.get_output_channel());

    Speaker out;
    out.set_input_channel(clip.get_output_channel());

    cout << "Entering playback loop..." << endl << endl;
    while(true)
    {
        out.consume();
    }

    
    return 0;
}
