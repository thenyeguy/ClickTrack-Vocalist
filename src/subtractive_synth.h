#ifndef SUBTRACTIVE_SYNTH_H
#define SUBTRACTIVE_SYNTH_H

#include "polyphonic_instrument.h"
#include "elementary_filters.h"
#include "oscillator.h"
#include "adsr.h"


namespace Instruments
{
    /* This is a subtractive synthesizer controlled over MIDI. It has support
     * for a varying number of oscillators.
     */
    class SubtractiveSynthVoice;
    class SubtractiveSynth : public PolyphonicInstrument
    {
        public:
            /* Constructor/destructor. Takes in how many oscillators to use, as
             * well as the MIDI channel
             */
            SubtractiveSynth(int oscillators=1, int midi_channel=-1);
            ~SubtractiveSynth();

            OutputChannel* get_output_channel();

        private:
            /* This contains our set of synth voices so we can free them
             */
            std::vector<SubtractiveSynthVoice> subtractive_voices;
            /* The rest of the signal chain follows...
             */
            Filters::GainFilter* gain;
    };


    class SubtractiveSynthVoice : public PolyphonicVoice
    {
        public:
            /* Constructor/destructor
             */
            SubtractiveSynthVoice(SubtractiveSynth* parent_synth);
            ~SubtractiveSynthVoice();

            /* Callbacks for starting and stopping notes
             */
            void handle_note_down(float velocity);
            void handle_note_up();
            void handle_pitch_wheel(float value);

            OutputChannel* get_output_channel();

        private:
            /* Define our signal chain
             */
            Oscillators::SawWave osc;
            Filters::ADSRFilter* adsr;
            Filters::GainFilter* gain;
    };
}


#endif
