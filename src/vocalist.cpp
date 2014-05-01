#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "vocalist.h"

using namespace ClickTrack;


Vocalist::Vocalist()
    : GenericInstrument(), 

      vibrato_lfo(Oscillator::Sine, 5),
      voice(Oscillator::BlepSaw, 220),
      noise(Oscillator::WhiteNoise, 0),
      tremelo_lfo(Oscillator::Sine, 5),
      tremelo(-12),

      note(0), pitch_multiplier(1.0),
      playing(false), sustained(false), held(false),

      attack_duration(2600),
      release_duration(10000),
      glide_duration(2000),
      interpolate_duration(500)
{
    /* Configure signal chain
     */
    tremelo.set_input_channel(AudioGenerator::get_output_channel());
    
    /* Configure LFOs
     */
    voice.set_lfo_input(vibrato_lfo.get_output_channel());
    voice.set_lfo_intensity(0.1);

    tremelo.set_lfo_input(tremelo_lfo.get_output_channel());
    tremelo.set_lfo_intensity(3.0);

    /* Load our sound sets
     */
    load_sound(A, "data/A.dat", gains[A], all_coeffs[A]);
    load_sound(E, "data/E.dat", gains[E], all_coeffs[E]);
    load_sound(I, "data/I.dat", gains[I], all_coeffs[I]);
    load_sound(O, "data/O.dat", gains[O], all_coeffs[O]);
    load_sound(U, "data/U.dat", gains[U], all_coeffs[U]);

    load_sound(V, "data/V.dat", gains[V], all_coeffs[V]);
    load_sound(Z, "data/Z.dat", gains[Z], all_coeffs[Z]);

    /* Initialize our containers
     */
    reflection_coeffs.push_back(0.0); //zeroth element never accessed
    reflection_coeffs_delta.push_back(0.0);
    forward_errors.push_back(0.0);
    backward_errors.push_back(0.0);

    /* Initialize our play state
     */
    attack_sound = V;
    held_sound = A;

    interpolating = false;
    current_state = SILENT;
    current_sound = A;
    gain = gains[A];
    gain_delta = 0;
    for(unsigned i = 0; i < num_coeffs; i++)
    {
        reflection_coeffs.push_back(all_coeffs[current_sound][i]);
        reflection_coeffs_delta.push_back(0.0);
        forward_errors.push_back(0.0);
        backward_errors.push_back(0.0);
    }
}


Channel* Vocalist::get_output_channel()
{
    return tremelo.get_output_channel();
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
                set_hold(A);
                break;
            case 39:
                std::cout << "Setting vowel to E" << std::endl;
                set_hold(E);
                break;
            case 42:
                std::cout << "Setting vowel to I" << std::endl;
                set_hold(I);
                break;
            case 44:
                std::cout << "Setting vowel to O" << std::endl;
                set_hold(O);
                break;
            case 46:
                std::cout << "Setting vowel to U" << std::endl;
                set_hold(U);
                break;

            case 36:
                std::cout << "Setting attack to H" << std::endl;
                set_attack(H);
                break;
            case 38:
                std::cout << "Setting attack to V" << std::endl;
                set_attack(V);
                break;
            case 40:
                std::cout << "Setting attack to F" << std::endl;
                set_attack(F);
                break;
            case 41:
                std::cout << "Setting attack to Z" << std::endl;
                set_attack(Z);
                break;
            case 43:
                std::cout << "Setting attack to S" << std::endl;
                set_attack(S);
                break;

            default:
                std::cout << "Ignoring sound change" << std::endl;
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
        handle_note_down(midiNoteToFreq(note));
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
            handle_note_up();
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
            handle_note_up();
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
            case 0x18: // attack time
            {
                float value = (float)message->at(2) / 127;
                attack_duration = value * 20000;
                break;
            }
            case 0x19: // glide time
            {
                float value = (float)message->at(2) / 127;
                glide_duration = value * 20000;
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


void Vocalist::handle_note_down(float target_freq)
{
    // Set the frequency
    // Handle state transition
    switch(current_state)
    {
        case ATTACK:
        case RELEASE:
        case SILENT:
            current_freq = target_freq;
            voice.set_freq(target_freq*pitch_multiplier);

            current_state = ATTACK;
            attack_time = get_next_time();
            break;

        case SUSTAIN:
            delta_freq = (target_freq - current_freq) / glide_duration;

            gliding = true;
            glide_time = get_next_time();
            break;
    }

    // Set the current sound
    current_sound = attack_sound;
}


void Vocalist::handle_note_up()
{
    current_state = RELEASE;
    release_time = get_next_time();
}


void Vocalist::generate_outputs(std::vector<SAMPLE>& output, unsigned long t)
{
    // Feed the lattice with input
    SAMPLE voiced = voice.get_output_channel()->get_sample(t);
    SAMPLE unvoiced = noise.get_output_channel()->get_sample(t);
    SAMPLE out;
    SAMPLE envelope = 1.0;
    switch(current_state)
    {
        case ATTACK:
        {
            // Check for state transition
            unsigned attack_t = t - attack_time;
            if(attack_t >= attack_duration)
            {
                current_state = SUSTAIN;
                current_sound = held_sound;
            }

            float alpha = 1.0*attack_t / attack_duration;

            // For H, fade in noise with voicing, cross fade in middle
            out = (alpha > 0.4 ? (alpha-0.4)/0.6 : 0.0)*voiced + 
                1.0/gain * (alpha > 0.6 ? 1.0 : alpha/0.6) * (1-alpha)*unvoiced;
            break;
        }

        case SUSTAIN:
        {
            out = voiced;
            break;
        }

        case RELEASE:
        {
            unsigned release_t = t - release_time;
            if(release_t >= release_duration)
                current_state = SILENT;

            envelope = 1 - ((float) release_t) / release_duration;
            out = voiced;
            break;
        }

        case SILENT:
        {
            envelope = 0.0;
            out = 0.0;
            break;
        }
    }

    // Handle pitch gliding
    if(gliding)
    {
        // Check for state transition
        unsigned glide_t = t - glide_time;
        if(glide_t >= glide_duration)
            gliding = false;

        // Continue gliding
        current_freq += delta_freq;
        voice.set_freq(current_freq*pitch_multiplier);
    }

    // Handle reflection coefficient updates
    if(interpolating)
    {
        // Check for state change
        if(t-interpolate_time >= interpolate_duration)
            interpolating = false;

        for(unsigned i = 1; i <= num_coeffs; i++)
            reflection_coeffs[i] += reflection_coeffs_delta[i];
        gain += gain_delta;
    }

    // Propogate the errors through the lattice
    forward_errors[num_coeffs] = out;
    for(unsigned i = num_coeffs; i > 0; i--)
    {
        forward_errors[i-1] = forward_errors[i] + 
            reflection_coeffs[i] * backward_errors[i-1];
        backward_errors[i] = -reflection_coeffs[i]*forward_errors[i-1] + 
            backward_errors[i-1];
    }

    // Write the sample out
    backward_errors[0] = forward_errors[0];
    output[0] = forward_errors[0] * gain / 20 * envelope;
}


void Vocalist::set_hold(Sound sound)
{
    // Determine interpolation
    switch(current_state)
    {
        case ATTACK:
        case RELEASE:
        case SILENT:
            gain = gains[sound];
            for(unsigned i = 0; i < num_coeffs; i++)
                reflection_coeffs[i+1] = all_coeffs[sound][i];
            break;

        case SUSTAIN:
            interpolating = true;
            interpolate_time = get_next_time();

            for(unsigned i = 0; i < num_coeffs; i++)
                reflection_coeffs_delta[i+1] = 
                   (all_coeffs[sound][i] - reflection_coeffs[i+1]) / 
                   interpolate_duration;
            gain_delta = (gains[sound] - gain) / interpolate_duration;

            break;
    }

    // Set sound
    held_sound = sound;
}


void Vocalist::set_attack(Sound sound)
{
    attack_sound = sound;
}


void Vocalist::load_sound(Sound sound, std::string file, float& gain,
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
