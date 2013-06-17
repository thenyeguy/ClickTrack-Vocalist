#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "filtergenerics.h"


namespace Oscillators
{
    class Oscillator : public FilterGenerics::AudioGenerator
    {
        protected:
            unsigned next_i; // next sample time
            float freq;      //hz

        public:
            Oscillator(float in_freq);

            void generate_outputs(SAMPLE** outputs);

            virtual float f(float t) = 0;
    };


    class SinWave : public Oscillator
    {
        public:
            SinWave(float in_freq);
            float f(float t);
    };


    class SquareWave : public Oscillator
    {
        public:
            SquareWave(float in_freq);
            float f(float t);
    };


    class TriangleWave : public Oscillator
    {
        public:
            TriangleWave(float in_freq);
            float f(float t);
    };
}

#endif
