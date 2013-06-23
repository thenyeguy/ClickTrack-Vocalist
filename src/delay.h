#ifndef DELAY_H
#define DELAY_H

#include <vector>
#include "filter_generics.h"
#include "ringbuffer.h"

using namespace FilterGenerics;


namespace Filters
{
    /* The simple delay is a small sample of an AudioFilter. It simply mirrors a
     * copy of its inputs, delayed one second.
     */
    class SimpleDelay : public AudioFilter
    {
        public:
            SimpleDelay(OutputChannel** in_input_channels,
                        unsigned in_num_channels = 1);

        private:
            void filter(SAMPLE** input, SAMPLE** output);


            std::vector<ClickTrackUtils::RingBuffer<SAMPLE> > last_second;
    };
}

#endif
