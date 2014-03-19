#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "audio_generics.h"
#include "scheduler.h"


namespace ClickTrack
{
    /* An oscillator is a basic generator unit. It handles periodic waveforms at
     * arbitrary frequencies, and supports many modes of operation
     */
    class Oscillator : public AudioGenerator
    {
        friend class FunctionScheduler<Oscillator>;

        public:
            /* The oscillator supports many modes.
             * Blep oscillators use PolyBlep to generate alias-free waveforms
             */
            enum OscMode { Sine, Saw, Square, Tri, WhiteNoise, 
                BlepSaw, BlepSquare, BlepTri};
            Oscillator(float in_freq, OscMode mode);

            /* Sets the mode
             */
            void set_mode(OscMode mode);

            /* Sets the frequency; uses a function scheduler to trigger this
             * event at the specified time. If no time is given, applies
             * immediately
             */
            void set_freq(float freq, unsigned long time=0);

        protected:
            /* This callback can only be set by the scheduler
             */
            static void set_freq_callback(Oscillator& caller, void* payload);

        private: 
            /* Overridden method for AudioGenerator to provide basic time
             * tracking and output for oscillators
             *
             * PolyBLEP oscillators use a periodic offset to remove aliasing
             */
            void generate_outputs(std::vector<SAMPLE>& outputs, unsigned long t);
            float polyBlepOffset(float t);
            float last_output; // used by blep triangle

            /* Used to schedule frequency changes
             */
            FunctionScheduler<Oscillator> scheduler;

            /* Phase state
             */
            float phase;     // rads
            float phase_inc; // rads

            /* Oscillator state
             */
            OscMode mode;
            float freq;      // hz

    };
}

#endif
