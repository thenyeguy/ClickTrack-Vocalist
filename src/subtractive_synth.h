#ifndef SUBTRACTIVE_SYNTH_H
#define SUBTRACTIVE_SYNTH_H

#include "polyphonic_instrument.h"
#include "elementary_filters.h"
#include "oscillator.h"
#include "adsr.h"


namespace Instruments
{
    /* This is a subtractive synthesizer controlled over MIDI. It is polyphonic
     */
    class SubtractiveSynthVoice;
    class SubtractiveSynth : public PolyphonicInstrument
    {
        public:
            /* Constructor/destructor. Takes in how many oscillators to use, as
             * well as the MIDI channel
             */
            SubtractiveSynth(int oscillators=1, int midi_channel=-1);

            /* Override because we have added more signal chain
             */
            Channel* get_output_channel();

        private:
            /* Add a gain to prevent clipping
             */
            Filters::GainFilter gain;
    };


    class SubtractiveSynthVoice : public PolyphonicVoice
    {
        public:
            /* Constructor/destructor
             */
            SubtractiveSynthVoice(SubtractiveSynth* parent_synth);

            /* Callbacks for starting and stopping notes
             */
            void handle_note_down(float velocity);
            void handle_note_up();
            void handle_pitch_wheel(float value);

            Channel* get_output_channel();

        private:
            /* Define our signal chain
             */
            Oscillators::SawWave osc;
            Filters::ADSRFilter adsr;
            Filters::GainFilter gain;
    };
}


#endif
