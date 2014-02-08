#ifndef POLYPHONIC_INSTRUMENT_H
#define POLYPHONIC_INSTRUMENT_H

#include <list>
#include <map>
#include "generic_instrument.h"
#include "filter_generics.h"
#include "elementary_filters.h"

namespace Instruments
{
    class PolyphonicVoice;
    class PolyphonicInstrument : public GenericInstrument
    {
        friend class PolyphonicVoice;

        public:
            PolyphonicInstrument(int voices, int midi_channel=-1);
            ~PolyphonicInstrument();

            /* By default, the instrument feeds all its voices into an adder and
             * returns that as the output. If there is a signal chain after the
             * adder, you must override the following function to return the
             * output channel
             */
            virtual Channel* get_output_channel();

        protected:
            /* The constructor for an inherited class must call this function to
             * add its voices to our internal queues.
             */
            void add_voices(std::vector<PolyphonicVoice*>& voices);

            /* The following callbacks are used to trigger and update the state
             * of our voices. They are entirely handled by this generic class
             */
            void on_note_down(unsigned note, float velocity);
            void on_note_up(unsigned note, float velocity);
            void on_sustain_down();
            void on_sustain_up();
            void on_pitch_wheel(unsigned value);

            /* Other MIDI messages vary from instrument to instrument. This can
             * be overriden to handle them
             */
            virtual void on_midi_message(std::vector<unsigned char>* message);

            /* Used by the voice to signal that its note is done playing.
             */ 
            void voice_done(PolyphonicVoice* voice);

            /* Sums the output of our voices
             */
            Filters::Adder adder;

        private:
            /* Voices are tracked in two lists. First, all voices are kept in
             * all_voices, in ascending order of when they were last triggered.
             * Then, voices that aren't playing are kept in free_voice. This
             * allows us to first take unused voices, and then recycle in the
             * order they were triggered.
             *
             * In the map, voices are indexed by the MIDI note number
             * identifying them.
             */
            unsigned num_voices;
            std::list<PolyphonicVoice*> all_voices;
            std::list<PolyphonicVoice*> free_voices;
            std::map<unsigned, PolyphonicVoice*> note_to_voice;
    };


    class PolyphonicVoice
    {
        friend class PolyphoncInstrument;

        public:
            PolyphonicVoice(PolyphonicInstrument* parent);
            virtual ~PolyphonicVoice() {}

            /* Callbacks for starting and stopping notes. The superclass
             * will call the handlers to set audio properties
             */
            void on_note_down(unsigned note, float velocity);
            void on_note_up();
            void on_sustain_down();
            void on_sustain_up();
            void on_pitch_wheel(float value);

            virtual void handle_note_down(float velocity) = 0;
            virtual void handle_note_up() = 0;
            virtual void handle_pitch_wheel(float value) = 0;

            /* The output channel must defined by the subclass, as we do not
             * know the signal chain
             */
            virtual Channel* get_output_channel() = 0;

            /* Returns if this voice is playing now
             */
            bool is_playing();

            /* Returns the midi value of the playing note
             */
            unsigned get_note();

        protected:
            PolyphonicInstrument* parent;

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
    };
}

#endif
