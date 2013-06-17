#ifndef DELAY_H
#define DELAY_H

#include <vector>
#include "filtergenerics.h"
#include "ringbuffer.h"

using namespace FilterGenerics;


namespace Filters
{
    class SimpleDelay : public AudioFilter
    {
        private:
            std::vector<ClickTrackUtils::RingBuffer<SAMPLE> > last_second;

        public:
            SimpleDelay(unsigned in_num_channels,
                        OutputChannel* in_input_channels);

            void filter(SAMPLE** input, SAMPLE** output);
    };
}

#endif
