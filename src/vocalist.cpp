#include <cmath>
#include <iostream>
#include "vocalist.h"

using namespace ClickTrack;


Vocalist::Vocalist()
    : GenericInstrument(), 
      vibrato_lfo(Oscillator::Sine, 5),
      voice(Oscillator::PulseTrain, 220),
      noise(Oscillator::WhiteNoise, 0),
      tremelo_lfo(Oscillator::Sine, 5),
      tremelo(-INFINITY),
      note(0), pitch_multiplier(1.0),
      playing(false), sustained(false), held(false)
{
    // Configure signal chain
    tremelo.set_input_channel(voice.get_output_channel());
    
    // Configure LFOs
    voice.set_lfo_input(vibrato_lfo.get_output_channel());
    voice.set_lfo_intensity(0.0);

    tremelo.set_lfo_input(tremelo_lfo.get_output_channel());
    tremelo.set_lfo_intensity(0.0);
}


Channel* Vocalist::get_output_channel()
{
    return tremelo.get_output_channel();
}


void Vocalist::on_note_down(unsigned in_note, float velocity, unsigned long time)
{
    // Update the state
    note = in_note;
    held = true;
    playing = true;

    // Trigger the changes
    voice.set_freq(midiNoteToFreq(note) * pitch_multiplier);
    tremelo.set_gain(0.0);
}


void Vocalist::on_note_up(unsigned in_note, float velocity, unsigned long time)
{
    // Ignore other notes
    if(note != in_note)
        return;
    
    // If we are holding the key down...
    if(held)
    {
        held = false;
        if(playing && !sustained)
        {
            playing = false;
            tremelo.set_gain(-INFINITY);
        }
    }
}


void Vocalist::on_sustain_down(unsigned long time)
{
    sustained = true;
}


void Vocalist::on_sustain_up(unsigned long time)
{
    // If we are sustaining...
    if(sustained)
    {
        sustained = false;
        if(playing && !held) 
        {
            playing = false;
            tremelo.set_gain(-INFINITY);
        }
    }
}


void Vocalist::on_pitch_wheel(unsigned value, unsigned long time)
{
    // Convert pitch wheel value to signed representation
    // Then allow a max bend of one step
    int centered = value - 0x2000;
    pitch_multiplier = pow(2, ((float)centered / 0x2000) * 2.0/12.0);

    // Apply the bend
    voice.set_freq(midiNoteToFreq(note) * pitch_multiplier);
}


void Vocalist::on_midi_message(std::vector<unsigned char>* message,
        unsigned long time)
{
    std::cout << "Midi message" << std::endl;
}
