#ifndef SIMPLE_MIDI_INSTRUMENT_H
#define SIMPLE_MIDI_INSTRUMENT_H

#include <list>
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
    class SubtractiveSynthOsc;
    class SubtractiveSynth : public GenericInstrument
    {
        friend class SubtractiveSynthOsc;

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
            void on_pitch_wheel(unsigned value);
            void on_midi_message(std::vector<unsigned char>* message);
            

            void osc_done(SubtractiveSynthOsc* osc);

        private:
            /* Oscillators are tracked in two lists. First, all oscillators are
             * kept in all_oscs, in ascending order of when they were last
             * triggered. Then, oscillators that aren't playing are kept in
             * free_oscs. This allows us to first take unused oscillators, and
             * then recycle in the order they were triggered.
             *
             * In the map, oscillators are indexed by the MIDI note number
             * identifying them.
             */
            const unsigned num_oscs;
            std::list<SubtractiveSynthOsc*> all_oscs;
            std::list<SubtractiveSynthOsc*> free_oscs;
            std::map<unsigned, SubtractiveSynthOsc*> note_to_osc;

            /* The rest of the signal chain follows...
             */
            Filters::Adder* sum;
            Filters::GainFilter* gain;
    };


    class SubtractiveSynthOsc
    {
        friend class SubtractiveSynth;
        
        public:
            /* Constructor/destructor
             */
            SubtractiveSynthOsc(SubtractiveSynth* parent_synth);
            ~SubtractiveSynthOsc();

            OutputChannel* get_output_channel();

            /* Returns if this oscillator is playing now
             */
            bool is_playing();

            /* Returns the midi value of the playing note
             */
            unsigned get_note();

            /* Callbacks for starting and stopping notes
             */
            void on_note_down(unsigned note, float velocity);
            void on_note_up();
            void on_sustain_down();
            void on_sustain_up();
            void on_pitch_wheel(float value);

        private:
            SubtractiveSynth* parent_synth;

            /* The MIDI note value being played
             */
            unsigned note;
            float freq;
            float pitch_multiplier;

            /* Current play status
             */
            bool playing;
            bool sustained;
            bool held;

            Oscillators::SawWave osc;
            Filters::ADSRFilter* adsr;
            Filters::GainFilter* gain;
    };
}


#endif
