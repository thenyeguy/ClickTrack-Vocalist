#ifndef GENERIC_INSTRUMENT_H
#define GENERIC_INSTRUMENT_H

#include "filter_generics.h"
#include "midi_wrapper.h"


namespace Instruments
{
    /* The Generic Instrument is an abstract class that defines the interface
     * for a MIDI instrument;
     */
    class GenericInstrument
    {
        friend class Midi::MidiIn;

        public:
            /* Constructor/destructor. If no channel is specified, the user is
             * asked what channel to use
             */
            GenericInstrument(int midi_channel=-1);
            ~GenericInstrument();

            /* Exposes the final OutputChannels of the instrument, so that it
             * may be plugged later into the signal chain.
             */
            FilterGenerics::OutputChannel* get_output_channel(int channel=0);
            const unsigned get_num_output_channels();

        protected:
            /* The following two functions are called by our MIDI callback. They
             * are responsible for handling the keypresses of our MIDI
             * instrument. Must be overrideen
             */
            virtual void on_note_down(unsigned note, float velocity) = 0;
            virtual void on_note_up(unsigned note, float velocity) = 0;

            /* Used by subclasses to add their own output channels
             */
            void add_output_channel(FilterGenerics::OutputChannel* channel);

        private:
            /* A vector of all our output channels. The subclass msut push their
             * output channels into this vector.
             */
            std::vector<FilterGenerics::OutputChannel*> output_channels;

            /* This is our MIDI listener. The MIDI callback will call our
             * instrument functions.
             */ 
            Midi::MidiIn midi;
    };
}

#endif