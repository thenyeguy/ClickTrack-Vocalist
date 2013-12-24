#include "subtractive_synth.h"


using namespace std;
using namespace Filters;
using namespace Oscillators;
using namespace Instruments;


SubtractiveSynthNote::SubtractiveSynthNote()
    : osc(440)
{
    OutputChannel* ch = osc.get_output_channel();
    adsr = new ADSRFilter(.1, .1, .5, .1, &ch);
}


SubtractiveSynthNote::~SubtractiveSynthNote()
{
    delete adsr;
}


OutputChannel* SubtractiveSynthNote::get_output_channel()
{
    return adsr->get_output_channel();
}


void SubtractiveSynthNote::on_note_down(float freq, float velocity)
{
    osc.set_freq(freq);
    adsr->on_note_down();
}


void SubtractiveSynthNote::on_note_up()
{
    adsr->on_note_up();
}


SubtractiveSynth::SubtractiveSynth(int oscillators, int channel)
    : GenericInstrument(channel), num_oscs(oscillators),
      all_oscs(), free_oscs(), playing_oscs()
{
    // Add all our oscillators to the free queue to start
    OutputChannel** outputs = new OutputChannel*[num_oscs];
    for(int i=0; i < num_oscs; i++)
    {
        SubtractiveSynthNote* osc = new SubtractiveSynthNote();

        all_oscs.push_back(osc);
        free_oscs.push(osc);

        outputs[i] = osc->get_output_channel();
    }

    // Create adder and gain, and use its output
    sum = new Filters::Adder(outputs, num_oscs);

    OutputChannel* sumCh = sum->get_output_channel();
    gain = new Filters::GainFilter(0.3, & sumCh);
    add_output_channel(gain->get_output_channel()); 
}


SubtractiveSynth::~SubtractiveSynth()
{
    for(int i=0; i < num_oscs; i++)
    {
        delete all_oscs[i];
    }
    delete sum;
    delete gain;
}


void SubtractiveSynth::on_note_down(unsigned note, float velocity)
{
    // To play a note, we need an oscillator to trigger 
    SubtractiveSynthNote* osc;
    if(!free_oscs.empty())
    {
        // If there are free oscillators, simply take on off the queue
        osc = free_oscs.front();
        free_oscs.pop();
    }
    else
    {
        // Otherwise, find the oldest note triggered and take it
        unsigned old_note = playing_notes.front();
        playing_notes.pop_front();

        osc = playing_oscs[old_note];
        playing_oscs.erase(old_note);
    }

    // Mark this oscillator as used again
    playing_oscs[note] = osc; playing_notes.push_back(note);

    // Trigger it and continue
    osc->on_note_down(Midi::noteToFreq(note), velocity);
}


void SubtractiveSynth::on_note_up(unsigned note, float velocity)
{
    // Get the oscillator, pause it and mark it as free if playing
    SubtractiveSynthNote* osc = playing_oscs[note];
    if(osc != NULL)
    {
        osc->on_note_up();
        free_oscs.push(osc);

        // Remove this note from the map
        playing_oscs.erase(note);

        // Remove it from the queue
        playing_notes.remove(note);
    }
}
