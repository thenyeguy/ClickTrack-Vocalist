#include "subtractive_synth.h"
#include <iterator>


using namespace std;
using namespace Filters;
using namespace Oscillators;
using namespace Instruments;


SubtractiveSynthNote::SubtractiveSynthNote()
    :  note(0), playing(false), sustained(false), held(false), osc(440)
{
    OutputChannel* ch = osc.get_output_channel();
    adsr = new ADSRFilter(.1, .1, .8, .1, &ch);
}


SubtractiveSynthNote::~SubtractiveSynthNote()
{
    delete adsr;
}


OutputChannel* SubtractiveSynthNote::get_output_channel()
{
    return adsr->get_output_channel();
}


bool SubtractiveSynthNote::is_playing()
{
    return playing;
}


unsigned SubtractiveSynthNote::get_note()
{
    return note;
}


void SubtractiveSynthNote::on_note_down(unsigned in_note, float velocity)
{
    held = true;
    playing = true;
    note = in_note;

    osc.set_freq(Midi::noteToFreq(note));
    adsr->on_note_down();
}


void SubtractiveSynthNote::on_note_up()
{
    if(held)
    {
        held = false;
        if(playing && !sustained)
        {
            playing = false;
            note = 0;
            adsr->on_note_up();
        }
    }
}


void SubtractiveSynthNote::on_sustain_down()
{
    sustained = true;
}


void SubtractiveSynthNote::on_sustain_up()
{
    if(sustained)
    {
        sustained = false;
        if(playing && !held) 
        {
            playing = false;
            note = 0;
            adsr->on_note_up();
        }
    }
}


SubtractiveSynth::SubtractiveSynth(int oscillators, int channel)
    : GenericInstrument(channel), num_oscs(oscillators),
      all_oscs(), free_oscs(), note_to_osc()
{
    // Add all our oscillators to the free queue to start
    OutputChannel** outputs = new OutputChannel*[num_oscs];
    for(int i=0; i < num_oscs; i++)
    {
        SubtractiveSynthNote* osc = new SubtractiveSynthNote();

        all_oscs.push_back(osc);
        free_oscs.push_back(osc);

        outputs[i] = osc->get_output_channel();
    }

    // Configure filter chain
    // Create adder and gain, and use its output
    sum = new Filters::Adder(outputs, num_oscs);

    OutputChannel* sumCh = sum->get_output_channel();
    gain = new Filters::GainFilter(0.3, & sumCh);

    add_output_channel(gain->get_output_channel()); 
}


SubtractiveSynth::~SubtractiveSynth()
{
    for(auto osc : all_oscs)
    {
        delete osc;
    }
    delete sum;
    delete gain;
}


void SubtractiveSynth::on_note_down(unsigned note, float velocity)
{
    // To play a note, we need an oscillator to trigger. Pull the front
    // oscillator, move it to the back and use it
    SubtractiveSynthNote* osc;
    if(free_oscs.empty())
    {
        osc = all_oscs.front();
        all_oscs.pop_front();
        all_oscs.push_back(osc);
        
        note_to_osc.erase(osc->get_note());
    }
    else
    {
        osc = free_oscs.front();
        free_oscs.pop_front();
    }

    // Trigger it and continue
    note_to_osc[note] = osc;
    osc->on_note_down(note, velocity);
}


void SubtractiveSynth::on_note_up(unsigned note, float velocity)
{
    // Get the oscillator, release and mark it as free if done playing
    SubtractiveSynthNote* osc = note_to_osc[note];
    if(osc != NULL)
    {
        // Check if it was playing
        bool playing = osc->is_playing();

        // Trigger the keyup
        osc->on_note_up();

        // If we just finished, remove from the playing map
        if(playing && !osc->is_playing())
        {
            note_to_osc.erase(note);
            free_oscs.push_back(osc);
        }
    }
}


void SubtractiveSynth::on_sustain_down()
{
    for(auto osc : all_oscs)
        osc->on_sustain_down();
}



void SubtractiveSynth::on_sustain_up()
{
    for(auto osc : all_oscs)
    {
        // Check if it was playing
        bool playing = osc->is_playing();

        // Trigger the sustain up
        osc->on_sustain_up();

        // If we just finished, remove from the playing map
        if(playing && !osc->is_playing())
        {
            note_to_osc.erase(osc->get_note());
            free_oscs.push_back(osc);
        }
    }
}
