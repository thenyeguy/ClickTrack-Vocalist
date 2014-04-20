#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
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
    load_sound(VocalistFilter::A, "data/A.dat", gains[VocalistFilter::A],
            all_coeffs[VocalistFilter::A]);
    load_sound(VocalistFilter::E, "data/E.dat", gains[VocalistFilter::E],
            all_coeffs[VocalistFilter::E]);
    load_sound(VocalistFilter::I, "data/I.dat", gains[VocalistFilter::I],
            all_coeffs[VocalistFilter::I]);
    load_sound(VocalistFilter::O, "data/O.dat", gains[VocalistFilter::O],
            all_coeffs[VocalistFilter::O]);
    load_sound(VocalistFilter::U, "data/U.dat", gains[VocalistFilter::U],
            all_coeffs[VocalistFilter::U]);


    /* Initialize our containers
     */
    reflection_coeffs.push_back(0.0); //zeroth element never accessed
    forward_errors.push_back(0.0);
    backward_errors.push_back(0.0);

    /* Initialize our play state
     */
    playing = false;
    current_sound = VocalistFilter::A;
    gain = gains[VocalistFilter::A];
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


void VocalistFilter::on_note_down()
{
    playing = true;
}


void VocalistFilter::on_note_up()
{
    playing = false;
}


void VocalistFilter::load_sound(Sound sound, std::string file, float& gain,
        std::vector<float>& coeffs)
{
    // Open our file
    std::fstream coeffFile;
    coeffFile.open(file);

    // Read the header
    std::string name;
    coeffFile >> name; // ignore the name
    coeffFile >> gain;
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
    forward_errors[num_coeffs] = playing ? input[0] : 0.0;

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
    output[0] = forward_errors[0] * gain / 500;
}




Vocalist::Vocalist()
    : GenericInstrument(), 
      vibrato_lfo(Oscillator::Sine, 5),
      voice(Oscillator::BlepSaw, 220),
      noise(Oscillator::WhiteNoise, 0),
      voiceModel(),
      tremelo_lfo(Oscillator::Sine, 5),
      tremelo(-12),
      filter(SecondOrderFilter::LOWPASS, 20000),
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
        voiceModel.on_note_down();
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
            voiceModel.on_note_up();
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
            voiceModel.on_note_up();
        }
    }
}


void Vocalist::on_pitch_wheel(float value, unsigned long time)
{
    // Allow a max bend of one step
    pitch_multiplier = pow(2, value * 2.0/12.0);

    // Apply the bend
    voice.set_freq(midiNoteToFreq(note) * pitch_multiplier);
}

void Vocalist::on_modulation_wheel(float value, unsigned long time)
{
    // Mod wheel controls vibrato
    voice.set_lfo_intensity(value);
}


void Vocalist::on_midi_message(std::vector<unsigned char>* message,
        unsigned long time)
{
    // Handle sliders from my MIDI keyboard
    if(message->at(0) == 0xb0)
    {
        switch(message->at(1))
        {
            case 0x16: // volume
            {
                float value = (float)message->at(2) / 127;
                tremelo.set_gain((value - 1.0) * 20);
                break;
            }
            case 0x17: // tremelo
            {
                float value = (float)message->at(2) / 127;
                tremelo.set_lfo_intensity(value * 20);
                break;
            }
            default:
            {
                std::cout << "Ignoring control " << message->at(1) << std::endl;
            }
        }
    }
    else
    {
        std::cout << "Unknown messsage: 0x";
        for(int i=0; i < message->size(); i++)
            std::cout << std::hex << std::setfill('0') << std::setw(2) << 
                (unsigned) message->at(i);
        std::cout << std::endl;
    }

}
