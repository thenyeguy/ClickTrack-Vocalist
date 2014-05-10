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

      attack_modifier(1.0),
      attack_duration(0), // set based on consonant
      release_duration(4000),
      glide_duration(2000),
      held_interpolate_duration(500) //500
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
    load_sound(A, "data/A.dat");
    load_sound(E, "data/E.dat");
    load_sound(I, "data/I.dat");
    load_sound(O, "data/O.dat");
    load_sound(U, "data/U.dat");

    load_sound(V, "data/V.dat");
    load_sound(Z, "data/Z.dat");

    load_sound(M, "data/M.dat");
    load_sound(N, "data/N.dat");

    load_sound(T, "data/T.dat");
    load_sound(P, "data/P.dat");
    load_sound(K, "data/K.dat");

    /* Initialize our containers
     */
    gain = 0;
    gain_delta = 0;

    reflection_coeffs.push_back(0.0); //zeroth element never accessed
    reflection_coeffs_delta.push_back(0.0);
    forward_errors.push_back(0.0);
    backward_errors.push_back(0.0);

    for(unsigned i = 0; i < num_coeffs; i++)
    {
        reflection_coeffs.push_back(0.0);
        reflection_coeffs_delta.push_back(0.0);
        forward_errors.push_back(0.0);
        backward_errors.push_back(0.0);
    }

    /* Initialize our play state
     */
    playing = false;
    sustained = false;
    held = false;

    note = 0;
    pitch_multiplier = 1.0;

    gliding = false;
    interpolating = false;

    current_state = SILENT;

    /* Initialize our default sounds
     */
    set_hold(I);
    set_attack(H);
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
    if(26 <= in_note && in_note <= 47) // change sound
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
                std::cout << "Setting attack to T" << std::endl;
                set_attack(T);
                break;
            case 40:
                std::cout << "Setting attack to D" << std::endl;
                set_attack(D);
                break;
            case 41:
                std::cout << "Setting attack to P" << std::endl;
                set_attack(P);
                break;
            case 43:
                std::cout << "Setting attack to F" << std::endl;
                set_attack(F);
                break;
            case 45:
                std::cout << "Setting attack to V" << std::endl;
                set_attack(V);
                break;
            case 47:
                std::cout << "Setting attack to M" << std::endl;
                set_attack(M);
                break;

            case 35:
                std::cout << "Setting attack to N" << std::endl;
                set_attack(N);
                break;

            case 33:
                std::cout << "Setting attack to G" << std::endl;
                set_attack(G);
                break;

            case 31:
                std::cout << "Setting attack to K" << std::endl;
                set_attack(K);
                break;

            case 29:
                std::cout << "Setting attack to B" << std::endl;
                set_attack(B);
                break;

            case 28:
                std::cout << "Setting attack to Z" << std::endl;
                set_attack(Z);
                break;

            case 26:
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
            case 0x17: // vibrato
            {
                float value = (float)message->at(2) / 127;
                voice.set_lfo_intensity(value);
                break;
            }
            case 0x18: // tremelo
            {
                float value = (float)message->at(2) / 127;
                tremelo.set_lfo_intensity(value * 20);
                break;
            }
            case 0x19: // attack time
            {
                float value = (float)message->at(2) / 127;
                attack_modifier = value * 3.0;
                set_attack(attack_sound);
                break;
            }
            case 0x1A: // release time
            {
                float value = (float)message->at(2) / 127;
                release_duration = value * 20000 + 1;
                break;
            }
            case 0x1B: // glide time
            {
                float value = (float)message->at(2) / 127;
                glide_duration = value * 20000 + 1;
                break;
            }
            case 0x1C: // interpolate time
            {
                float value = (float)message->at(2) / 127;
                held_interpolate_duration = value * 20000 + 1;
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
        {
            // Handle frequnecy
            current_freq = target_freq;
            voice.set_freq(target_freq*pitch_multiplier);

            // Handle state transition
            current_state = ATTACK;
            attack_time = get_next_time();

            // Prepare the reflection coeffs
            Sound sound_coeffs;
            switch(attack_sound)
            {
                // Map voiceless fircatives to their voiced equivalents
                case F:
                    sound_coeffs = V;
                    break;
                case S:
                    sound_coeffs = Z;
                    break;
                // Map voiced stops to their voiceless equivalents
                case D:
                    sound_coeffs = T;
                    break;
                case B:
                    sound_coeffs = P;
                    break;
                case G:
                    sound_coeffs = K;
                    break;
                // Map H to its vowel
                case H:
                    sound_coeffs = held_sound;
                    break;
                default:
                    sound_coeffs = attack_sound;
                    break;
            }
            gain = gains[sound_coeffs];
            for(unsigned i = 0; i <= num_coeffs; i++)
                reflection_coeffs[i+1] = all_coeffs[sound_coeffs][i];

            break;
        }

        case SUSTAIN:
        {
            // Set up a glide to the new note
            delta_freq = (target_freq - current_freq) / glide_duration;
            gliding = true;
            glide_time = get_next_time();
            break;
        }
    }
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
    SAMPLE unvoiced = 1.0/gain * noise.get_output_channel()->get_sample(t);

    SAMPLE out = 0.0;
    SAMPLE envelope = 1.0;
    switch(current_state)
    {
        case ATTACK:
        {
            // Check for state transition
            unsigned attack_t = t - attack_time;
            if(attack_t >= attack_duration)
                current_state = SUSTAIN;

            float alpha = 1.0*attack_t / attack_duration;
            switch(attack_sound)
            {
                case H:
                    // Fade in noise with voicing, cross fade in middle
                    out = (alpha > 0.4 ? (alpha-0.4)/0.6 : 0.0)*voiced + 
                        (alpha > 0.6 ? 1.0 : alpha/0.6) * (1-alpha)*unvoiced;
                    break;

                case F:
                case S:
                    // Fade in noise with voicing, cross fade in middle
                    out = (alpha > 0.6 ? (alpha-0.6)/0.4 : 0.0)*voiced + 
                        0.7*(alpha > 0.4 ? 1.0 : alpha/0.4) * (1-alpha)*unvoiced;

                    // Trigger an early interpolation to target
                    if(!interpolating && alpha > 0.85)
                        interpolate_sound(held_sound, attack_duration*0.05);
                    break;

                case V:
                case Z:
                    // Frontload attack, mix in noise
                    out = (alpha < 0.2 ? alpha/0.2 : 1.0)*voiced + 
                        0.6*(alpha < 0.4 ? alpha/0.4 : 1.0) * (1-alpha)*unvoiced;

                    if(!interpolating && alpha > 0.6)
                        interpolate_sound(held_sound, attack_duration*0.2);
                    break;

                case M:
                case N:
                    out = (alpha < 0.2 ? alpha/0.2 : 1.0)*voiced;

                    if(!interpolating && alpha > 0.8)
                        interpolate_sound(held_sound, attack_duration*0.2);
                    break;

                case T:
                    // Start with noise, cross fade to voiced
                    out = (alpha > 0.4 ? (alpha-0.4)/0.6 : 0.0)*voiced + 
                       (1-sqrt(alpha))*unvoiced;

                    if(!interpolating && alpha > 0.7)
                        interpolate_sound(held_sound, attack_duration*0.3);
                    break;

                case K:
                    // Start with noise, cross fade to voiced
                    out = (alpha > 0.4 ? (alpha-0.4)/0.6 : 0.0)*voiced + 
                        0.8*(1-sqrt(alpha))*unvoiced;

                    if(!interpolating && alpha > 0.6)
                        interpolate_sound(held_sound, attack_duration*0.4);
                    break;

                case P:
                    // Start with noise, cross fade to voiced
                    out = (alpha > 0.4 ? (alpha-0.4)/0.6 : 0.0)*voiced + 
                        0.4*(1-sqrt(alpha))*unvoiced;

                    if(!interpolating && alpha > 0.6)
                        interpolate_sound(held_sound, attack_duration*0.4);
                    break;

                case D:
                    // Fade in noise with voicing, cross fade in middle
                    out = voiced + 
                       0.3*(1-sqrt(alpha))*unvoiced;

                    if(!interpolating && alpha > 0.3 && alpha < 0.4)
                        interpolate_sound(held_sound, 0.3*attack_duration);
                    break;

                case G:
                    // Fade in noise with voicing, cross fade in middle
                    out = voiced + 
                       0.1*(1-alpha)*unvoiced;

                    if(!interpolating && alpha > 0.5 && alpha < 0.6)
                        interpolate_sound(held_sound, 0.3*attack_duration);
                    break;

                case B:
                    // Fade in noise with voicing, cross fade in middle
                    out = voiced + 
                       0.3*(1-alpha)*unvoiced;

                    if(!interpolating && alpha > 0.6 && alpha < 0.7)
                        interpolate_sound(held_sound, 0.3*attack_duration);
                    break;

                default:
                    out = 0.0;
                    break;
            }
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
            interpolate_sound(sound, held_interpolate_duration);
            break;
    }

    // Set sound
    held_sound = sound;
}


void Vocalist::set_attack(Sound sound)
{
    attack_sound = sound;

    // Set attack time
    switch(sound)
    {
        case F:
            attack_duration = 6000;
            break;
        case Z:
            attack_duration = 6000;
            break;
        case S:
            attack_duration = 6000;
            break;
        case M:
        case N:
            attack_duration = 4500;
            break;
        case H:
        case T:
        case P:
        case K:
        case D:
        case B:
        case G:
        case V:
            attack_duration = 3200;
            break;
        default:
            attack_duration = 0;
            break;
    }
    attack_duration *= attack_modifier + 1;
}


void Vocalist::interpolate_sound(Sound sound, unsigned duration)
{
    interpolating = true;
    interpolate_time = get_next_time();
    interpolate_duration = duration;

    for(unsigned i = 0; i < num_coeffs; i++)
        reflection_coeffs_delta[i+1] = 
           (all_coeffs[sound][i] - reflection_coeffs[i+1]) / 
           interpolate_duration;
    gain_delta = (gains[sound] - gain) / interpolate_duration;
}


void Vocalist::load_sound(Sound sound, std::string file)
{
    // Open our file
    std::fstream coeffFile;
    coeffFile.open(file);

    // Read the header
    std::string name;
    coeffFile >> name; // ignore the name
    coeffFile >> gains[sound];
    coeffFile >> num_coeffs;

    // Read the coefficients into our vector
    for(unsigned i = 0; i < num_coeffs; i++)
    {
        float c;
        coeffFile >> c;
        all_coeffs[sound].push_back(c);
    }

    // Close the file
    coeffFile.close();
}
