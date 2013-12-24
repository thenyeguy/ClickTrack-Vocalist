#include "simple_midi_instrument.h"


using namespace Instruments;


SimpleMidiInstrument::SimpleMidiInstrument(int channel)
    : GenericInstrument(channel), playing(false), last_note(0), osc(440)
{
    osc.pause();
    add_output_channel(osc.get_output_channel());
}


SimpleMidiInstrument::~SimpleMidiInstrument()
{}


void SimpleMidiInstrument::on_note_down(unsigned note, float velocity)
{
    playing = true; last_note = note;
    osc.set_freq(Midi::noteToFreq(note));
    osc.unpause();
}


void SimpleMidiInstrument::on_note_up(unsigned note, float velocity)
{
    if(note == last_note)
    {
        playing = false;
        osc.pause();
    }
}
