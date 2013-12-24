#ifndef MIDI_WRAPPER_H
#define MIDI_WRAPPER_H

#include <rtmidi.h>


// Forward declare
namespace Instruments
{
    class GenericInstrument;
}

namespace Midi
{
    /* Converts a MIDI note number to a frequency
     */
    float noteToFreq(unsigned note);

    /* A wrapper for RtMIDI. It is registered directly to an instrument class
     * and handles all its playing through callbacks to the MIDI process. It
     * internally talks to its instrument to convey state.
     */
    class MidiIn
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
            MidiIn(Instruments::GenericInstrument* inst, int channel=-1);

            ~MidiIn();

        private:
            RtMidiIn stream;
            Instruments::GenericInstrument* inst;

            /* Callback for registering with the input stream
             * Parses the MIDI message and passes on its message to the
             * specified destination.
             */
            static void callback(double deltaTime,
                                 std::vector<unsigned char>* message,
                                 void* midi);
    };
}

#endif
