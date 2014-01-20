#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "filter_generics.h"


namespace Oscillators
{
    /* An oscillator is a basic generator unit. It internally keeps track of
     * time, and generates a periodic function "f". To implement your own
     * oscillator, simply provide a subclass that defines "f".
     *
     * EG a sine wave oscillator
     */
    class Oscillator : public FilterGenerics::AudioGenerator
    {
        public:
            Oscillator(float in_freq);

            bool is_paused();
            void pause();
            void unpause();

            void set_freq(float freq);

        protected:
            // Overridden method for AudioGenerator to provide basic time
            // tracking and output for oscillators
            void generate_outputs(SAMPLE** outputs);

            /* The workhorse of an oscillator. It uses the current phase of the
             * wave in radians to compute the value of the waveform.
             */
            virtual float f() = 0;
            float phase;     // rads

        private: 
            bool paused;
            float freq;      // hz
            float phase_inc; // rads

    };


    /* A simple sine wave oscillator
     */
    class SinWave : public Oscillator
    {
        public:
            SinWave(float in_freq);

        protected:
            float f();
    };


    /* A simple square wave oscillator
     */
    class SquareWave : public Oscillator
    {
        public:
            SquareWave(float in_freq);

        protected:
            float f();
    };


    /* A simple triangle wave oscillator
     */
    class TriangleWave : public Oscillator
    {
        public:
            TriangleWave(float in_freq);

        protected:
            float f();
    };


    /* A simple white noise "oscillator"
     * Has frequency for compatibility with other oscillators, but it is ignored
     * internally
     */
    class WhiteNoise : public Oscillator
    {
        public:
            WhiteNoise(float in_freq);

        protected:
            float f();
    };
}

#endif
