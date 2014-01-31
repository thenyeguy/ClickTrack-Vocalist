#ifndef DELAY_H
#define DELAY_H

#include <vector>
#include "filter_generics.h"
#include "ringbuffer.h"

using namespace FilterGenerics;


namespace Filters
{
    /* The delay filter plays back delayed copies of its input. The first delay
     * is played at full amplitude, and is then decayed linearly by the feedback
     * multiplier. The delayed signal is combined with a the original input by
     * the wetness percentage. The signal is delayed by delay_time seconds.
     */
    class Delay : public AudioFilter
    {
        public:
            Delay(float delay_time, float feedback, float wetness,
                  OutputChannel** in_input_channels, unsigned in_num_channels = 1);
            ~Delay();

            /* Setters for parameters.
             *
             * Note that setting the delay time clears the buffer.
             */
            void set_delay(float delay_time);
            void set_feedback(float feedback);
            void set_wetness(float wetness);

        private:
            void filter(SAMPLE** input, SAMPLE** output);

            unsigned delay;
            float feedback;
            float wetness;


            std::vector<ClickTrackUtils::RingBuffer<SAMPLE>*> delay_buffers;
    };
}

#endif
