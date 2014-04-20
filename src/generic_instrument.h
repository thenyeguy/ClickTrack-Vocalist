#ifndef GENERIC_INSTRUMENT_H
#define GENERIC_INSTRUMENT_H

#include "audio_generics.h"


namespace ClickTrack
{
    /* Converts a MIDI note number to a frequency
     */
    float midiNoteToFreq(unsigned note);


    /* The Generic Instrument is an abstract class that defines the interface
     * for a MIDI instrument class.
     *
     * The instrument has no means to trigger its own notes. Instead, it must be
     * attached to some listener (eg a MidiListener), that will then call the
     * event handlers.
     *
     * This attachment must be done at initialization time by the user.
     */
    class GenericInstrument
    {
        public:
            GenericInstrument();

            /* Exposes the final output channel of the instrument, so that it
             * may be plugged later into the signal chain.
             */
            Channel* get_output_channel(int channel=0);
            const unsigned get_num_output_channels();

            /* The following functions are called by the listener. They are
             * responsible for handling the messages sent to our instrument.
             * Must be overrideen.
             *
             * Some messages are parsed and identified by the MidiIn class. If
             * so, they use the named message types below. If a message is not
             * special handled, then the original message is passed directly to
             * the instrument using on_midi_message, for custom handling.
             *
             * All our handlers take a time to trigger, in sample time. A time
             * of zero means to trigger at the moment the message is received.
             */ 
            virtual void on_note_down(unsigned note, float velocity,
                    unsigned long time=0) = 0;
            virtual void on_note_up(unsigned note, float velocity,
                    unsigned long time=0) = 0;

            virtual void on_sustain_down(unsigned long time=0) = 0;
            virtual void on_sustain_up(unsigned long time=0) = 0;

            virtual void on_pitch_wheel(float value, 
                    unsigned long time=0) = 0;
            virtual void on_modulation_wheel(float value, 
                    unsigned long time=0) = 0;

            virtual void on_midi_message(std::vector<unsigned char>* message,
                    unsigned long time=0) = 0;

        protected:
            /* Used by subclasses to add their own output channels
             */
            void add_output_channel(Channel* channel);

        private:
            /* A vector of all our output channels. The subclass must push their
             * output channels into this vector.
             */
            std::vector<Channel*> output_channels;
    };
}

#endif
