#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "audio_generics.h"


namespace ClickTrack
{
    /* An oscillator is a basic generator unit. It internally keeps track of
     * time, and generates a periodic function "f". To implement your own
     * oscillator, simply provide a subclass that defines "f".
     *
     * EG a sine wave oscillator
     */
    class Oscillator : public AudioGenerator
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
            void generate_outputs(std::vector< std::vector<SAMPLE> >& outputs);

            /* The workhorse of an oscillator. It uses the current phase of the
             * wave in radians to compute the value of the waveform.
             */
            virtual float f() = 0;
            float phase;     // rads
            float phase_inc; // rads

        private: 
            bool paused;
            float freq;      // hz

    };


    /* A sine wave oscillator
     */
    class SinWave : public Oscillator
    {
        public:
            SinWave(float in_freq);
        protected:
            float f();
    };


    /* A white noise "oscillator"
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


    /* These simple Oscillators are calculated in a naive fashion.
     *
     * This causes them to alias in the frequency domain
     */
    class SimpleSawWave : public Oscillator
    {
        public:
            SimpleSawWave(float in_freq);
        protected:
            float f();
    };
    class SimpleSquareWave : public Oscillator
    {
        public:
            SimpleSquareWave(float in_freq);
        protected:
            float f();
    };
    class SimpleTriangleWave : public Oscillator
    {
        public:
            SimpleTriangleWave(float in_freq);
        protected:
            float f();
    };


    /* These sets of Oscillators are designed to not alias in the frequency
     * domain, and thus sound cleaner as pure tones.
     *
     * Implemented using a PolyBlep algorithm. This introduces a slight rolloff
     * at the sharp, discontinuous boundries of the waveform.
     */
    class PolyBlepOscillator : public Oscillator
    {
        public:
            PolyBlepOscillator(float in_freq);
        protected:
            float polyBlepOffset(float t);
    };
    class SawWave : public PolyBlepOscillator
    {
        public:
            SawWave(float in_freq);
        protected:
            float f();
    };
    class SquareWave : public PolyBlepOscillator
    {
        public:
            SquareWave(float in_freq);
        protected:
            float f();
    };
    class TriangleWave : public PolyBlepOscillator
    {
        public:
            TriangleWave(float in_freq);
        protected:
            float f();
        private:
            float last_output;
    };
}

#endif
