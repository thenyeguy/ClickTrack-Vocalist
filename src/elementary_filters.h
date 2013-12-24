#ifndef ELEMENTARYFILTERS_H
#define ELEMENTARYFILTERS_H

#include <vector>
#include "filter_generics.h"
#include "ringbuffer.h"

using namespace FilterGenerics;


namespace Filters
{
    /* The unity filter is the simplest possible AudioFilter. It simply directly
     * mirrors its input to its output.
     */
    class UnityFilter : public AudioFilter
    {
        public:
            UnityFilter(OutputChannel** in_input_channels,
                        unsigned in_num_channels = 1);

        private:
            void filter(SAMPLE** input, SAMPLE** output);
    };


    /* The gain filter takes a multiplier coefficient, and multiplies all its
     * inputs by the gain factor.
     */
    class GainFilter : public AudioFilter
    {
        public:
            GainFilter(float in_gain,
                       OutputChannel** in_input_channels,
                       unsigned in_num_channels = 1);

        private:
            void filter(SAMPLE** input, SAMPLE** output);


            float gain;
    };


    /* The adder takes in a set of inputs and returns a single output that is
     * the sum of all its inputs
     */
    class Adder : public AudioFilter
    {
        public:
            Adder();
            Adder(OutputChannel** in_input_channels,
                  unsigned in_num_channels);

        private:
            void filter(SAMPLE** input, SAMPLE** output);
    };
}

#endif
