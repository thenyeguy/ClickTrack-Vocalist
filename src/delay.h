#ifndef DELAY_H
#define DELAY_H

#include <vector>
#include "filtergenerics.h"
#include "ringbuffer.h"

using namespace FilterGenerics;


namespace Filters
{
    /* The simple delay is a small sample of an AudioFilter. It simply mirrors a
     * copy of its inputs, delayed one second.
     */
    class SimpleDelay : public AudioFilter
    {
        private:
            std::vector<ClickTrackUtils::RingBuffer<SAMPLE> > last_second;

        public:
            SimpleDelay(OutputChannel* in_input_channels,
                        unsigned in_num_channels = 1);

            void filter(SAMPLE** input, SAMPLE** output);
    };
}

#endif
