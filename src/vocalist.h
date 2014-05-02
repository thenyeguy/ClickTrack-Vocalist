#ifndef VOCALIST_H
#define VOCALIST_H

#include <map>
#include <string>
#include "audio_generics.h"
#include "gain_filter.h"
#include "generic_instrument.h"
#include "oscillator.h"

namespace ClickTrack
{
    /* The vocalist is an instrument that functions like a voice
     */
    class Vocalist : public GenericInstrument, AudioGenerator
    {
        public:
            Vocalist();

            Channel* get_output_channel();

            /* The following callbacks are used to trigger and update the state
             * of our voices. They are entirely handled by this generic class.
             */
            void on_note_down(unsigned note, float velocity, unsigned long time=0);
            void on_note_up(unsigned note, float velocity, unsigned long time=0);
            void on_sustain_down(unsigned long time=0);
            void on_sustain_up(unsigned long time=0);
            void on_pitch_wheel(float value, unsigned long time=0);
            void on_modulation_wheel(float value, unsigned long time=0);

            void handle_note_down(float target_freq);
            void handle_note_up();

            /* Other MIDI messages vary from instrument to instrument. This can
             * be overriden to handle them
             */
            void on_midi_message(std::vector<unsigned char>* message,
                    unsigned long time=0);

        private:
            /* Override the generator.
             */
            void generate_outputs(std::vector<SAMPLE>& output, unsigned long t);

            /* Helper function for changing sound sets
             */
            enum Sound { A, E, I, O, U, 
                V, F, Z, S, H };
            void set_hold(Sound sound);
            void set_attack(Sound sound);

            /* Helper to trigger an interpolation of the reflection coefficients
             */
            void interpolate_sound(Sound sound, unsigned duration);

            /* Helper function for loading sounds during initialization
             */
            void load_sound(Sound sound, std::string file, float& gain,
                    std::vector<float>& coeffs);

            /* Define our signal chain
             */
            Oscillator vibrato_lfo;
            Oscillator voice;
            Oscillator noise;

            GainFilter tremelo_lfo;
            GainFilter tremelo;

            /* Current note status
             */
            unsigned note;
            float pitch_multiplier;

            Sound attack_sound;
            Sound held_sound;

            /* Current play status
             */
            bool playing;
            bool sustained;
            bool held;

            /* Store ADSRish parameters
             */
            unsigned attack_duration;
            unsigned release_duration;
            unsigned glide_duration;
            unsigned held_interpolate_duration;

            /* Store sets of reflection coeffs for each vowel
             */
            unsigned num_coeffs;
            std::map<Sound, std::vector<float> > all_coeffs;
            std::map<Sound, float> gains;

            /* Store ADSRish state
             */
            enum State { ATTACK, SUSTAIN, RELEASE, SILENT };
            State current_state;

            unsigned long attack_time;
            unsigned long release_time;;

            bool gliding;
            unsigned long glide_time;
            float current_freq;
            float delta_freq;

            bool interpolating;
            unsigned interpolate_duration;
            unsigned interpolate_time;
            float gain_delta;
            std::vector<SAMPLE> reflection_coeffs_delta;

            /* Store filter coefficients for the lattice
             */
            float gain;
            std::vector<float> reflection_coeffs;
            std::vector<SAMPLE> forward_errors;
            std::vector<SAMPLE> backward_errors;
    };
}

#endif

