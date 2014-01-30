#include "subtractive_synth.h"
#include <iterator>
#include <cmath>


using namespace std;
using namespace Filters;
using namespace Oscillators;
using namespace Instruments;


SubtractiveSynth::SubtractiveSynth(int oscillators, int channel)
    : GenericInstrument(channel), num_oscs(oscillators),
      all_oscs(), free_oscs(), note_to_osc()
{
    // Add all our oscillators to the free queue to start
    OutputChannel** outputs = new OutputChannel*[num_oscs];
    for(int i=0; i < num_oscs; i++)
    {
        SubtractiveSynthOsc* osc = new SubtractiveSynthOsc(this);

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
    // To play a note, we need an oscillator to trigger.
    // If this note is already playing, stop it first.
    // Then pull the front oscillator, move it to the back and use it
    SubtractiveSynthOsc* osc;
    if(note_to_osc[note] != NULL)
    {
        osc = note_to_osc[note];
        all_oscs.remove(osc);
        all_oscs.push_back(osc);
    }
    else if(free_oscs.empty())
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
    SubtractiveSynthOsc* osc = note_to_osc[note];
    if(osc != NULL)
        osc->on_note_up();
}


void SubtractiveSynth::on_sustain_down()
{
    for(auto osc : all_oscs)
        osc->on_sustain_down();
}


void SubtractiveSynth::on_sustain_up()
{
    for(auto osc : all_oscs)
        osc->on_sustain_up();
}


void SubtractiveSynth::on_pitch_wheel(unsigned value)
{
    // Convert pitch wheel value to signed representation
    // Then allow a max bend of one step
    int centered = value - 0x2000;
    float bend = pow(2, ((float)centered / 0x2000) * 2.0/12.0);

    // Then apply to all oscillators
    for(auto osc : all_oscs) 
        osc->on_pitch_wheel(bend);
}


void SubtractiveSynth::osc_done(SubtractiveSynthOsc* osc)
{
    note_to_osc.erase(osc->get_note());
    free_oscs.push_back(osc);
}





SubtractiveSynthOsc::SubtractiveSynthOsc(SubtractiveSynth* in_parent_synth)
    :  parent_synth(in_parent_synth), note(0), freq(0.0), pitch_multiplier(1.0),
       playing(false), sustained(false), held(false), osc(440)
{
    OutputChannel* ch = osc.get_output_channel();
    adsr = new ADSRFilter(.005, .3, .5, .3, &ch);
    ch = adsr->get_output_channel();
    gain = new GainFilter(1.0, &ch);
}


SubtractiveSynthOsc::~SubtractiveSynthOsc()
{
    delete adsr;
}


OutputChannel* SubtractiveSynthOsc::get_output_channel()
{
    return gain->get_output_channel();
}


bool SubtractiveSynthOsc::is_playing()
{
    return playing;
}


unsigned SubtractiveSynthOsc::get_note()
{
    return note;
}


void SubtractiveSynthOsc::on_note_down(unsigned in_note, float velocity)
{
    held = true;
    playing = true;
    note = in_note;

    // Set oscillator state
    freq = Midi::noteToFreq(note);
    osc.set_freq(freq*pitch_multiplier);

    // Set velocity gain
    // TODO: change this curve
    gain->set_gain(pow(velocity,0.5));

    // Trigger ADSR
    adsr->on_note_down();
}


void SubtractiveSynthOsc::on_note_up()
{
    if(held)
    {
        held = false;
        if(playing && !sustained)
        {
            playing = false;
            adsr->on_note_up();

            parent_synth->osc_done(this);
            note = 0;
        }
    }
}


void SubtractiveSynthOsc::on_sustain_down()
{
    sustained = true;
}


void SubtractiveSynthOsc::on_sustain_up()
{
    if(sustained)
    {
        sustained = false;
        if(playing && !held) 
        {
            playing = false;
            adsr->on_note_up();

            parent_synth->osc_done(this);
            note = 0;
        }
    }
}


void SubtractiveSynthOsc::on_pitch_wheel(float value)
{
    pitch_multiplier = value;
    osc.set_freq(freq * pitch_multiplier);
}
