#ifndef VOCALIST_H
#define VOCALIST_H

#include <map>
#include <string>
#include "audio_generics.h"
#include "first_order_filter.h"
#include "second_order_filter.h"
#include "gain_filter.h"
#include "generic_instrument.h"
#include "oscillator.h"
#include "ringbuffer.h"

namespace ClickTrack
{
    /* The following implements a basic drum machine. It triggers samples based
     * on a configuration file, called keymap.txt
     *
     * The config file consists of one sample per line in the following format:
     *      MidiNumber path/to/your sample.wav
     * Blank lines and lines starting with # are ignored.
     */
    class VocalistFilter : public AudioFilter
    {
        friend class Vocalist;

        public:
            VocalistFilter();

        private:
            void filter(std::vector<SAMPLE>& input, 
                    std::vector<SAMPLE>& output, unsigned long t);

            /* Store filter coefficients for different vowels
             */
            unsigned num_coeffs;
            std::vector<float> reflection_coeffs;
            std::vector<SAMPLE> forward_errors;
            std::vector<SAMPLE> backward_errors;
    };

    class Vocalist : public GenericInstrument
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
            void on_pitch_wheel(unsigned value, unsigned long time=0);

            /* Other MIDI messages vary from instrument to instrument. This can
             * be overriden to handle them
             */
            void on_midi_message(std::vector<unsigned char>* message,
                    unsigned long time=0);

        private:
            /* Define our signal chain
             */
            Oscillator vibrato_lfo;
            Oscillator voice;
            Oscillator noise;

            VocalistFilter voiceModel;

            GainFilter tremelo_lfo;
            GainFilter tremelo;

            SecondOrderFilter filter;

            /* Current note status
             */
            unsigned note;
            float pitch_multiplier;

            /* Current play status
             */
            bool playing;
            bool sustained;
            bool held;
    };
}

#endif

