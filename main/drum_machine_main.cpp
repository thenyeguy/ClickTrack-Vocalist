#include "../src/basic_elements.h"
#include "../src/drum_machine.h"
#include "../src/io_elements.h"
#include "../src/midi_wrapper.h"

using namespace ClickTrack;

int main()
{
    using namespace std;

    cout << "Initializing MIDI instrument" << endl;
    DrumMachine drum("samples/roland808/");
    MidiListener midi(&drum, 1);

    cout << "Initializing signal chain" << endl;
    ClipDetector clip(1.0);
    clip.set_input_channel(drum.get_output_channel());

    Speaker out;
    out.set_input_channel(clip.get_output_channel());
    out.register_callback(MidiListener::consumer_callback, &midi);

    cout << "Entering playback loop..." << endl << endl;
    while(true)
    {
        out.consume();
    }

    return 0;
}
