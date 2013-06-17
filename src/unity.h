#ifndef UNITY_H
#define UNITY_H

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
}

#endif
