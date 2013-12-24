#ifndef SIMPLE_MIDI_INSTRUMENT_H
#define SIMPLE_MIDI_INSTRUMENT_H

#include "filter_generics.h"
#include "oscillator.h"
#include "generic_instrument.h"


namespace Instruments
{
    /* This is an instrument that is triggered by MIDI input. It is a very
     * simple monophonic, single channel subtractive synthesizer
     */
    class SimpleMidiInstrument : public GenericInstrument
    {
        public:
            /* Constructor/destructor. They have no arguments, and establish
             * the entire filter chain
             */
            SimpleMidiInstrument(int channel=-1);
            ~SimpleMidiInstrument();

        protected:
            void on_note_down(unsigned note, float velocity);
            void on_note_up(unsigned note, float velocity);

        private:
            bool playing;
            unsigned last_note;

            Oscillators::TriangleWave osc;
    };
}


#endif
