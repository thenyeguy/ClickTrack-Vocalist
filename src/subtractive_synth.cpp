#include "subtractive_synth.h"
#include <iterator>
#include <cmath>


using namespace ClickTrack;


SubtractiveSynth::SubtractiveSynth(int num_voices)
    : PolyphonicInstrument(num_voices), eq()
{
    // Initialize our voices
    std::vector<PolyphonicVoice*> voices;
    for(unsigned i = 0; i < num_voices; i++)
    {
        voices.push_back(new SubtractiveSynthVoice(this));
    }
    add_voices(voices);

    // Configure our signal chain
    eq.set_input_channel(PolyphonicInstrument::get_output_channel());

    // Default the eq to quiet so it doesn't clip
    eq.set_gain(0.3);
}

Channel* SubtractiveSynth::get_output_channel()
{
    return eq.get_output_channel();
}




SubtractiveSynthVoice::SubtractiveSynthVoice(SubtractiveSynth* in_parent_synth)
    :  PolyphonicVoice(in_parent_synth), osc(440), adsr(.005, .3, .5, .3)
{
    // Connect signal chain
    adsr.set_input_channel(osc.get_output_channel());
}


Channel* SubtractiveSynthVoice::get_output_channel()
{
    return adsr.get_output_channel();
}


void SubtractiveSynthVoice::handle_note_down(float velocity, unsigned long time)
{

    // Set velocity gain
    // TODO: change this curve
    adsr.set_gain(pow(velocity,0.5));

    // Trigger frequency and ADSR change
    osc.set_freq(freq*pitch_multiplier, time);
    adsr.on_note_down(time);
}


void SubtractiveSynthVoice::handle_note_up(unsigned long time)
{
    adsr.on_note_up(time);
}


void SubtractiveSynthVoice::handle_pitch_wheel(float value, unsigned long time)
{
    osc.set_freq(freq*pitch_multiplier, time);
}
