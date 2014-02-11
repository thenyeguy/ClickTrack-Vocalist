#ifndef POLYPHONIC_INSTRUMENT_CPP
#define POLYPHONIC_INSTRUMENT_CPP

#include <math.h>
#include "polyphonic_instrument.h"

using namespace Instruments;


PolyphonicInstrument::PolyphonicInstrument(int in_num_voices, int midi_channel)
    : GenericInstrument(midi_channel), adder(in_num_voices),
      num_voices(in_num_voices), all_voices(), free_voices(), note_to_voice()
{}


PolyphonicInstrument::~PolyphonicInstrument()
{
    for(auto voice : all_voices)
        delete voice;
}


Channel* PolyphonicInstrument::get_output_channel()
{
    return adder.get_output_channel();
}


void PolyphonicInstrument::add_voices(std::vector<PolyphonicVoice*>& voices)
{
    // Count our voices
    num_voices = voices.size();

    // Add all our oscillators to the free queue to start
    for(unsigned i = 0; i < num_voices; i++)
    {
        all_voices.push_back(voices[i]);
        free_voices.push_back(voices[i]);
        adder.set_input_channel(voices[i]->get_output_channel(),i);
    }
}


void PolyphonicInstrument::on_note_down(unsigned note, float velocity)
{
    // To play a note, we need a voice to trigger.
    // If this note is already playing, stop it first.
    // Then pull the front voice, move it to the back and use it
    PolyphonicVoice* voice;
    if(note_to_voice[note] != NULL)
    {
        voice = note_to_voice[note];
        all_voices.remove(voice);
        all_voices.push_back(voice);
    }
    else if(free_voices.empty())
    {
        voice = all_voices.front();
        all_voices.pop_front();
        all_voices.push_back(voice);
        
        note_to_voice.erase(voice->get_note());
    }
    else
    {
        voice = free_voices.front();
        free_voices.pop_front();
    }

    // Trigger it and continue
    note_to_voice[note] = voice;
    voice->on_note_down(note, velocity);
}


void PolyphonicInstrument::on_note_up(unsigned note, float velocity)
{
    // Get the voice, release and mark it as free if done playing
    PolyphonicVoice* voice = note_to_voice[note];
    if(voice != NULL)
        voice->on_note_up();
}


void PolyphonicInstrument::on_sustain_down()
{
    for(auto voice : all_voices)
        voice->on_sustain_down();
}


void PolyphonicInstrument::on_sustain_up()
{
    for(auto voice : all_voices)
        voice->on_sustain_up();
}


void PolyphonicInstrument::on_pitch_wheel(unsigned value)
{
    // Convert pitch wheel value to signed representation
    // Then allow a max bend of one step
    int centered = value - 0x2000;
    float bend = pow(2, ((float)centered / 0x2000) * 2.0/12.0);

    // Then apply to all voices
    for(auto voice : all_voices) 
        voice->on_pitch_wheel(bend);
}


void PolyphonicInstrument::on_midi_message(
        std::vector<unsigned char>* message)
{
    // Do nothing
    return;
}


void PolyphonicInstrument::voice_done(PolyphonicVoice* voice)
{
    note_to_voice.erase(voice->get_note());
    free_voices.push_back(voice);
}




PolyphonicVoice::PolyphonicVoice(PolyphonicInstrument* in_parent)
    :  parent(in_parent), note(0), pitch_multiplier(1.0),
       playing(false), sustained(false), held(false)
{}


void PolyphonicVoice::on_note_down(unsigned in_note, float velocity)
{
    held = true;
    playing = true;
    note = in_note;
    
    freq = Midi::noteToFreq(note);
    handle_note_down(velocity);
}


void PolyphonicVoice::on_note_up()
{
    if(held)
    {
        held = false;
        if(playing && !sustained)
        {
            playing = false;
            handle_note_up();

            parent->voice_done(this);
            note = 0;
        }
    }
}


void PolyphonicVoice::on_sustain_down()
{
    sustained = true;
}


void PolyphonicVoice::on_sustain_up()
{
    if(sustained)
    {
        sustained = false;
        if(playing && !held) 
        {
            playing = false;
            handle_note_up();

            parent->voice_done(this);
            note = 0;
        }
    }
}


void PolyphonicVoice::on_pitch_wheel(float value)
{
    pitch_multiplier = value;
    handle_pitch_wheel(value);
}


bool PolyphonicVoice::is_playing()
{
    return playing;
}


unsigned PolyphonicVoice::get_note()
{
    return note;
}

#endif