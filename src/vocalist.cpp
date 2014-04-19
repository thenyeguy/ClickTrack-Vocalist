#include <cmath>
#include <fstream>
#include <iostream>
#include "vocalist.h"

using namespace ClickTrack;


VocalistFilter::VocalistFilter()
    : AudioFilter(2,1),
      all_coeffs(),
      reflection_coeffs(),
      forward_errors(),
      backward_errors()
{
    /* Load our sound sets
     */
    load_sound(VocalistFilter::A, "data/A.dat", 
            all_coeffs[VocalistFilter::A]);
    load_sound(VocalistFilter::E, "data/E.dat", 
            all_coeffs[VocalistFilter::E]);
    load_sound(VocalistFilter::I, "data/I.dat", 
            all_coeffs[VocalistFilter::I]);
    load_sound(VocalistFilter::O, "data/O.dat", 
            all_coeffs[VocalistFilter::O]);
    load_sound(VocalistFilter::U, "data/U.dat", 
            all_coeffs[VocalistFilter::U]);

    current_sound = VocalistFilter::A;

    /* Initialize our containers
     */
    reflection_coeffs.push_back(0.0); //zeroth element never accessed
    forward_errors.push_back(0.0);
    backward_errors.push_back(0.0);

    for(unsigned i = 0; i < num_coeffs; i++)
    {
        reflection_coeffs.push_back(all_coeffs[current_sound][i]);
        forward_errors.push_back(0.0);
        backward_errors.push_back(0.0);
    }
}


void VocalistFilter::set_sound(Sound sound)
{
    current_sound = sound;

    for(unsigned i = 0; i < num_coeffs; i++)
        reflection_coeffs[i+1] = all_coeffs[current_sound][i];
}


void VocalistFilter::load_sound(Sound sound, std::string file,
        std::vector<float>& coeffs)
{
    // Open our file
    std::fstream coeffFile;
    coeffFile.open(file);

    // Read the header
    std::string name;
    coeffFile >> name; // ignore the name
    coeffFile >> num_coeffs;

    // Read the coefficients into our vector
    for(unsigned i = 0; i < num_coeffs; i++)
    {
        float c;
        coeffFile >> c;
        coeffs.push_back(c);
    }

    // Close the file
    coeffFile.close();
}


void VocalistFilter::filter(std::vector<SAMPLE>& input, 
        std::vector<SAMPLE>& output, unsigned long t)
{
    // Feed the lattice with input
    forward_errors[num_coeffs] = input[0];

    // Propogate the errors through the lattice
    for(unsigned i = num_coeffs; i > 0; i--)
    {
        forward_errors[i-1] = forward_errors[i] + 
            reflection_coeffs[i] * backward_errors[i-1];
        backward_errors[i] = -reflection_coeffs[i]*forward_errors[i-1] + 
            backward_errors[i-1];
    }

    // Write the sample out
    backward_errors[0] = forward_errors[0];
    output[0] = forward_errors[0] / 20;
}




Vocalist::Vocalist()
    : GenericInstrument(), 
      vibrato_lfo(Oscillator::Sine, 5),
      voice(Oscillator::PulseTrain, 220),
      noise(Oscillator::WhiteNoise, 0),
      voiceModel(),
      tremelo_lfo(Oscillator::Sine, 5),
      tremelo(-INFINITY),
      filter(SecondOrderFilter::LOWPASS, 5000),
      note(0), pitch_multiplier(1.0),
      playing(false), sustained(false), held(false)
{
    // Configure signal chain
    voiceModel.set_input_channel(voice.get_output_channel(),0);
    voiceModel.set_input_channel(noise.get_output_channel(),1);

    tremelo.set_input_channel(voiceModel.get_output_channel());

    filter.set_input_channel(tremelo.get_output_channel());
    
    // Configure LFOs
    voice.set_lfo_input(vibrato_lfo.get_output_channel());
    voice.set_lfo_intensity(0.1);

    tremelo.set_lfo_input(tremelo_lfo.get_output_channel());
    tremelo.set_lfo_intensity(3.0);
}


Channel* Vocalist::get_output_channel()
{
    return filter.get_output_channel();
}


void Vocalist::on_note_down(unsigned in_note, float velocity, unsigned long time)
{
    // Check what note was playing and decide our action
    // The second octave is mapped to changing the vocal sounds
    // The third octave (C3 to E4) is or performance range
    if(36 <= in_note && in_note <= 47) // change sound
    {
        // Change the note sound based on the key
        switch(in_note)
        {
            case 37:
                std::cout << "Setting vowel to A" << std::endl;
                voiceModel.set_sound(VocalistFilter::A);
                break;
            case 39:
                std::cout << "Setting vowel to E" << std::endl;
                voiceModel.set_sound(VocalistFilter::E);
                break;
            case 42:
                std::cout << "Setting vowel to I" << std::endl;
                voiceModel.set_sound(VocalistFilter::I);
                break;
            case 44:
                std::cout << "Setting vowel to O" << std::endl;
                voiceModel.set_sound(VocalistFilter::O);
                break;
            case 46:
                std::cout << "Setting vowel to U" << std::endl;
                voiceModel.set_sound(VocalistFilter::U);
                break;
        }
    }
    else if(48 <= in_note && in_note <= 64) // sing note
    {
        // Update the state
        note = in_note;
        held = true;
        playing = true;

        // Trigger the changes
        voice.set_freq(midiNoteToFreq(note) * pitch_multiplier);
        tremelo.set_gain(0.0);
    }
    else // alert out of range
    {
        std::cout << "Ignoring MIDI note: " << in_note << std::endl;
    }

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
