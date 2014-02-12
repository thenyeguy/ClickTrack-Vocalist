#include "subtractive_synth.h"
#include <iterator>
#include <cmath>


using namespace ClickTrack;


SubtractiveSynth::SubtractiveSynth(int num_voices, int midi_channel)
    : PolyphonicInstrument(num_voices, midi_channel), gain(0.3)
{
    // Initialize our voices
    std::vector<PolyphonicVoice*> voices;
    for(unsigned i = 0; i < num_voices; i++)
    {
        voices.push_back(new SubtractiveSynthVoice(this));
    }
    add_voices(voices);

    // Configure our signal chain
    gain.set_input_channel(PolyphonicInstrument::get_output_channel());
}

Channel* SubtractiveSynth::get_output_channel()
{
    return gain.get_output_channel();
}




SubtractiveSynthVoice::SubtractiveSynthVoice(SubtractiveSynth* in_parent_synth)
    :  PolyphonicVoice(in_parent_synth), osc(440), adsr(.005, .3, .5, .3),
       gain(1.0)
{
    // Connect signal chain
    adsr.set_input_channel(osc.get_output_channel());
    gain.set_input_channel(adsr.get_output_channel());
}


Channel* SubtractiveSynthVoice::get_output_channel()
{
    return gain.get_output_channel();
}


void SubtractiveSynthVoice::handle_note_down(float velocity)
{
    osc.set_freq(freq*pitch_multiplier);

    // Set velocity gain
    // TODO: change this curve
    gain.set_gain(pow(velocity,0.5));

    // Trigger ADSR
    adsr.on_note_down();
}


void SubtractiveSynthVoice::handle_note_up()
{
    adsr.on_note_up();
}


void SubtractiveSynthVoice::handle_pitch_wheel(float value)
{
    osc.set_freq(freq*pitch_multiplier);
}
