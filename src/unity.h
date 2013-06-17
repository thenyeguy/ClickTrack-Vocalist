#ifndef UNITY_H
#define UNITY_H

#include <vector>
#include "filtergenerics.h"
#include "ringbuffer.h"

using namespace FilterGenerics;


namespace Filters
{
    class UnityFilter : public AudioFilter
    {
        public:
            UnityFilter(unsigned in_num_channels,
                        OutputChannel* in_input_channels);

            void filter(SAMPLE** input, SAMPLE** output);
    };
}

#endif
