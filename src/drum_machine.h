#ifndef DRUM_MACHINE_H
#define DRUM_MACHINE_H

#include <map>
#include <string>
#include "adder.h"
#include "audio_generics.h"
#include "gain_filter.h"
#include "generic_instrument.h"
#include "scheduler.h"

namespace ClickTrack
{
    /* The following implements a basic drum machine. It triggers samples based
     * on a configuration file, called keymap.txt
     *
     * The config file consists of one sample per line in the following format:
     *      MidiNumber path/to/your sample.wav
     * Blank lines and lines starting with # are ignored.
     */
    class DrumVoice;
    class DrumAdder;
    class DrumMachine : public GenericInstrument
    {
        friend class DrumVoice;

        public:
            DrumMachine(const std::string& path);
            ~DrumMachine();

            Channel* get_output_channel();

            /* Reloads all voices from a new path
             */
            void set_voice(const std::string& path);

            /* Make the final gain filter public to expose volume changer
             */
            GainFilter volume;

            /* The following callbacks are used to trigger and update the state
             * of our voices. They are entirely handled by this generic class.
             */
            void on_note_down(unsigned note, float velocity, unsigned long time=0);
            void on_note_up(unsigned note, float velocity, unsigned long time=0);
            void on_sustain_down(unsigned long time=0);
            void on_sustain_up(unsigned long time=0);
            void on_pitch_wheel(float value, unsigned long time=0);
            void on_modulation_wheel(float value, unsigned long time=0);

            /* Other MIDI messages vary from instrument to instrument. This can
             * be overriden to handle them
             */
            void on_midi_message(std::vector<unsigned char>* message,
                    unsigned long time=0);

        private:
            /* Sums our different voices
             */
            DrumAdder* adder;
    };


    /* Drum adder is an adder with less overhead for large numbers of elements.
     */
    class DrumAdder : public AudioGenerator
    {
        friend class DrumMachine;

        protected:
            DrumAdder();
            ~DrumAdder();

            /* Used to change the tones on our voices
             */
            void set_voice(const std::string& path);

            /* Looks through the voices to determine which are playing
             */
            void generate_outputs(std::vector<SAMPLE>& outputs, unsigned long t);

            /* Callback to trigger a note. Only called by a drum machine.
             *
             * The function is scheduled, and will use the function scheduler to
             * trigger the callback handler, which is passed the note as
             * a payload.
             */
            void on_note_down(unsigned note, float velocity, unsigned long t);
            static void handle_note_down(DrumAdder& caller, void* payload);

            /* Scheduler for note downs
             */
            FunctionScheduler<DrumAdder> scheduler;

            /* The following map simply maps MIDI numbers to their drum voice
             */
            std::map<unsigned, DrumVoice*> voices;
    };


    /* Drum voice manages a single sample in a drum machine. It reads its sample
     * into memory for playback. It will retrigger its sound on repeated note
     * down.
     */
    class DrumVoice
    {
        friend class DrumAdder;

        protected:
            DrumVoice(const std::string& filename);

            /* Handle getting notes
             */
            SAMPLE get_next_sample();
            void on_note_down();

            /* Returns true if playing
             */
            bool is_playing();

            /* State for audio buffer
             */
            std::vector<SAMPLE> samples;
            unsigned next_i;
            bool playing;
    };


    class InvalidKeymap: public std::exception
    {
        public:
        InvalidKeymap(const std::string in_filename)
            : filename(in_filename) {}

        const std::string filename;
        virtual const char* what() const throw()
        {
            std::string message = 
                "DrumMachine could not open the specified keymap: " + filename;
            return message.c_str();
        }
    };
}

#endif

