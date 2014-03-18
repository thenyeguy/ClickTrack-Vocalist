#include "subtractive_synth.h"
#include <iterator>
#include <cmath>


using namespace ClickTrack;


SubtractiveSynth::SubtractiveSynth(int num_voices)
    : PolyphonicInstrument(num_voices), eq()
{
    // Initialize our voices
    std::vector<PolyphonicVoice*> temp;
    for(unsigned i = 0; i < num_voices; i++)
    {
        SubtractiveSynthVoice* voice = new SubtractiveSynthVoice(this);
        voices.push_back(voice);
        temp.push_back(voice);
    }
    add_voices(temp);

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
    : PolyphonicVoice(in_parent_synth), osc1(440, Oscillator::Saw), 
      osc2(440, Oscillator::Saw), adder(2), adsr(.005, .3, .5, .3)
{
    // Connect signal chain
    adder.set_input_channel(osc1.get_output_channel(), 0);
    adder.set_input_channel(osc2.get_output_channel(), 1);
    adsr.set_input_channel(adder.get_output_channel());
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
    osc1.set_freq(freq*pitch_multiplier, time);
    osc2.set_freq(freq*pitch_multiplier, time);
    adsr.on_note_down(time);
}


void SubtractiveSynthVoice::handle_note_up(unsigned long time)
{
    adsr.on_note_up(time);
}


void SubtractiveSynthVoice::handle_pitch_wheel(float value, unsigned long time)
{
    osc1.set_freq(freq*pitch_multiplier, time);
    osc2.set_freq(freq*pitch_multiplier, time);
}
