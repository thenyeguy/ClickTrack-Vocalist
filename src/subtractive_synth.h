#ifndef SIMPLE_MIDI_INSTRUMENT_H
#define SIMPLE_MIDI_INSTRUMENT_H

#include <list>
#include <queue>
#include <map>
#include "filter_generics.h"
#include "elementary_filters.h"
#include "oscillator.h"
#include "adsr.h"
#include "generic_instrument.h"


namespace Instruments
{
    /* This is a subtractive synthesizer controlled over MIDI. It has support
     * for a varying number of oscillators.
     */
    class SubtractiveSynthNote
    {
        public:
            /* Constructor/destructor
             */
            SubtractiveSynthNote();
            ~SubtractiveSynthNote();

            OutputChannel* get_output_channel();

            void on_note_down(float freq, float velocity);
            void on_note_up();

        private:
            Oscillators::TriangleWave osc;
            Filters::ADSRFilter* adsr;
    };


    class SubtractiveSynth : public GenericInstrument
    {
        public:
            /* Constructor/destructor. Takes in how many oscillators to use, as
             * well as the MIDI channel
             */
            SubtractiveSynth(int oscillators=1, int midi_channel=-1);
            ~SubtractiveSynth();

        protected:
            void on_note_down(unsigned note, float velocity);
            void on_note_up(unsigned note, float velocity);

            void on_sustain_down();
            void on_sustain_up();

        private:
            /* Oscillators are tracked as either available (i.e. paused), or
             * currently playing. All oscillators are either in the free queue,
             * or in the map of playing oscillators. In the map, oscillators are
             * indexed by the MIDI note number identifying them.
             */
            const unsigned num_oscs;
            std::vector<SubtractiveSynthNote*> all_oscs;

            std::queue<SubtractiveSynthNote*> free_oscs;

            std::list<unsigned> playing_notes;
            std::map<unsigned, SubtractiveSynthNote*> playing_oscs;

            /* Tracks internal state from control signals
             */
            bool sustained;

            /* The rest of the signal chain follows...
             */
            Filters::Adder* sum;
            Filters::GainFilter* gain;
    };
}


#endif
