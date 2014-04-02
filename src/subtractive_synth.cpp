#include <iterator>
#include <cmath>
#include "subtractive_synth.h"

using namespace ClickTrack;


SubtractiveSynth::SubtractiveSynth(int num_voices)
    : PolyphonicInstrument(num_voices), 
      filter(SecondOrderFilter::LOWPASS, 20000),
      lfo(Oscillator::Sine, 5),
      volume(-10)
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
    filter.set_input_channel(PolyphonicInstrument::get_output_channel());
    volume.set_input_channel(filter.get_output_channel());

    // Connect the LFO
    for(auto voice : voices)
    {
        voice->osc1.set_lfo_input(lfo.get_output_channel());
        voice->osc2.set_lfo_input(lfo.get_output_channel());
    }
    volume.set_lfo_input(lfo.get_output_channel());
}


Channel* SubtractiveSynth::get_output_channel()
{
    return volume.get_output_channel();
}


void SubtractiveSynth::set_gain(float in_gain)
{
    volume.set_gain(in_gain);
}


void SubtractiveSynth::set_osc1_mode(Oscillator::Mode mode)
{
    for(auto voice : voices)
        voice->osc1.set_mode(mode);
}


void SubtractiveSynth::set_osc2_mode(Oscillator::Mode mode)
{
    for(auto voice : voices)
        voice->osc2.set_mode(mode);
}


void SubtractiveSynth::set_osc1_transposition(float steps)
{
    for(auto voice : voices)
        voice->osc1.set_transposition(steps);
}


void SubtractiveSynth::set_osc2_transposition(float steps)
{
    for(auto voice : voices)
        voice->osc2.set_transposition(steps);
}


void SubtractiveSynth::set_attack_time(float attack_time)
{
    for(auto voice : voices)
        voice->adsr.set_attack_time(attack_time);
}


void SubtractiveSynth::set_decay_time(float decay_time)
{
    for(auto voice : voices)
        voice->adsr.set_decay_time(decay_time);
}


void SubtractiveSynth::set_sustain_level(float sustain_level)
{
    for(auto voice : voices)
        voice->adsr.set_sustain_level(sustain_level);
}


void SubtractiveSynth::set_release_time(float release_time)
{
    for(auto voice : voices)
        voice->adsr.set_release_time(release_time);
}


void SubtractiveSynth::set_lfo_vibrato(float steps)
{
    for(auto voice : voices)
    {
        voice->osc1.set_lfo_intensity(steps);
        voice->osc2.set_lfo_intensity(steps);
    }
}


void SubtractiveSynth::set_lfo_tremelo(float db)
{
    volume.set_lfo_intensity(db);
}




SubtractiveSynthVoice::SubtractiveSynthVoice(SubtractiveSynth* in_parent_synth)
    : PolyphonicVoice(in_parent_synth), osc1(Oscillator::Saw, 440), 
      osc2(Oscillator::Saw, 440), adder(2), adsr()
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
