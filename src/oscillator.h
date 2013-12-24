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

            /* The workhorse of an oscillator. It takes in an absolute time
             * t (in seconds), and uses it to compute the value of the waveform
             * at that time.
             */
            virtual float f(float t) = 0;

            unsigned next_i; // next sample time
            float freq;      //hz
            bool paused;
    };


    /* A simple sine wave oscillator
     */
    class SinWave : public Oscillator
    {
        public:
            SinWave(float in_freq);

        protected:
            float f(float t);
    };


    /* A simple square wave oscillator
     */
    class SquareWave : public Oscillator
    {
        public:
            SquareWave(float in_freq);

        protected:
            float f(float t);
    };


    /* A simple triangle wave oscillator
     */
    class TriangleWave : public Oscillator
    {
        public:
            TriangleWave(float in_freq);

        protected:
            float f(float t);
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
            float f(float t);
    };
}

#endif