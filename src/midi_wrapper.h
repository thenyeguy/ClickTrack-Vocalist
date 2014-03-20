#ifndef MIDI_WRAPPER_H
#define MIDI_WRAPPER_H

#include <chrono>
#include <rtmidi.h>
#include "generic_instrument.h"


namespace ClickTrack
{
    /* A wrapper for RtMIDI. It is registered directly to an instrument class
     * and handles all its playing through callbacks to the MIDI process. It
     * internally talks to its instrument to convey state.
     */
    class MidiListener
    {
        public:
            /* Constructor and destructor automatically open and close the
             * RtMidi streams for us.
             *
             * The MIDI listener requires an instrument to respond to in its
             * callback.
             *
             * If no channel is provided, the constructor asks which channel
             * to use.
             */
            MidiListener(GenericInstrument* inst, int channel=-1);

            /* Callback to pass to the speaker for timing management. The
             * payload should be a pointer to the MidiListener object
             */
            static void timing_callback(unsigned long time, void* payload);

        private:
            /* Callback for registering with the input stream
             * Parses the MIDI message and passes on its message to the
             * specified destination.
             */
            static void midi_callback(double deltaTime,
                    std::vector<unsigned char>* message, void* in_listener);

            /* State for MIDI
             */
            RtMidiIn stream;
            GenericInstrument* inst;

            /* State for timing and computing buffer offsets
             */
            std::chrono::time_point<std::chrono::high_resolution_clock,
                std::chrono::duration<double> > buffer_timestamp;
            unsigned long next_buffer_time;
    };
}

#endif
