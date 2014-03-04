#ifndef SUBTRACTIVE_SYNTH_H
#define SUBTRACTIVE_SYNTH_H

#include "adsr.h"
#include "equalizer.h"
#include "oscillator.h"
#include "polyphonic_instrument.h"


namespace ClickTrack
{
    /* This is a subtractive synthesizer controlled over MIDI. It is polyphonic
     */
    class SubtractiveSynth : public PolyphonicInstrument
    {
        public:
            /* Constructor/destructor. Takes in how many voices to use, as
             * well as the MIDI channel
             */
            SubtractiveSynth(int voices=1);

            /* Override because we have added more signal chain
             */
            Channel* get_output_channel();

            /* The synth is fed through an EQ at the end of the signal chain.
             * This EQ is public so as to expose its existing interface
             */
            FourPointEqualizer eq;
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
            SawWave osc;
            ADSRFilter adsr;
    };
}


#endif
