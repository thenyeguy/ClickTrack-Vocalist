#ifndef ELEMENTARYFILTERS_H
#define ELEMENTARYFILTERS_H

#include <vector>
#include "filtergenerics.h"
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
            UnityFilter(OutputChannel* in_input_channels,
                        unsigned in_num_channels = 1);

            void filter(SAMPLE** input, SAMPLE** output);
    };


    /* The gain filter takes a multiplier coefficient, and multiplies all its
     * inputs by the gain factor;
     */
    class GainFilter : public AudioFilter
    {
        private:
            float gain;
        public:
            GainFilter(float in_gain,
                       OutputChannel* in_input_channels,
                       unsigned in_num_channels = 1);

            void filter(SAMPLE** input, SAMPLE** output);
    };
}

#endif
