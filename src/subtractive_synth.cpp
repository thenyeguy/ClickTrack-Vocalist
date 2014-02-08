#include "subtractive_synth.h"
#include <iterator>
#include <cmath>


using namespace std;
using namespace Filters;
using namespace Oscillators;
using namespace Instruments;


SubtractiveSynth::SubtractiveSynth(int voices, int midi_channels)
    : PolyphonicInstrument(midi_channels)
{
    // Initialize our voices
    for(unsigned i = 0; i < voices; i++)
        subtractive_voices.push_back(SubtractiveSynthVoice(this));
    add_voices(&subtractive_voices);

    // Add all our oscillators to the free queue to start
    OutputChannel* sumCh = adder->get_output_channel();
    gain = new Filters::GainFilter(0.3, & sumCh);

    add_output_channel(gain->get_output_channel()); 
}


SubtractiveSynth::~SubtractiveSynth()
{
    delete gain;
}


OutputChannel* SubtractiveSynth::get_output_channel()
{
    return gain->get_output_channel();
}




SubtractiveSynthVoice::SubtractiveSynthVoice(SubtractiveSynth* in_parent_synth)
    :  PolyphonicVoice(in_parent_synth),  osc(440)
{
    OutputChannel* ch = osc.get_output_channel();
    adsr = new ADSRFilter(.005, .3, .5, .3, &ch);
    ch = adsr->get_output_channel();
    gain = new GainFilter(1.0, &ch);
}


SubtractiveSynthVoice::~SubtractiveSynthVoice()
{
    delete adsr;
    delete gain;
}


OutputChannel* SubtractiveSynthVoice::get_output_channel()
{
    return gain->get_output_channel();
}


void SubtractiveSynthVoice::handle_note_down(float velocity)
{
    osc.set_freq(freq*pitch_multiplier);

    // Set velocity gain
    // TODO: change this curve
    gain->set_gain(pow(velocity,0.5));

    // Trigger ADSR
    adsr->on_note_down();
}


void SubtractiveSynthVoice::handle_note_up()
{
    adsr->on_note_up();
}


void SubtractiveSynthVoice::handle_pitch_wheel(float value)
{
    osc.set_freq(freq*pitch_multiplier);
}
