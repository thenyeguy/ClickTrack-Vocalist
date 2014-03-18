#ifndef SUBTRACTIVE_SYNTH_H
#define SUBTRACTIVE_SYNTH_H

#include "adsr.h"
#include "equalizer.h"
#include "oscillator.h"
#include "polyphonic_instrument.h"


namespace ClickTrack
{
    /* This is a subtractive synthesizer controlled over MIDI. It is polyphonic.
     */
    class SubtractiveSynthVoice;
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

        private:
            std::vector<SubtractiveSynthVoice*> voices;
    };


    class SubtractiveSynthVoice : public PolyphonicVoice
    {
        friend class SubtractiveSynth;

        public:
            /* Constructor/destructor
             */
            SubtractiveSynthVoice(SubtractiveSynth* parent_synth);

            /* Gets the output of this voice
             */
            Channel* get_output_channel();

            /* Callbacks for starting and stopping notes
             */
            void handle_note_down(float velocity, unsigned long time);
            void handle_note_up(unsigned long time);
            void handle_pitch_wheel(float value, unsigned long time);

            /* Voice selection for each oscillator - each oscillator can be one
             * of several modes
             */

        protected:
            /* Define our signal chain
             */
            Oscillator osc1, osc2;
            
            Adder adder;
            ADSRFilter adsr;
    };
}


#endif
