#ifndef LIMITER_H
#define LIMITER_H

#include "level_detector.h"
#include "ringbuffer.h"


namespace ClickTrack
{
    /* The limiter takes in a threshold in decibels, and then limits the
     * amplitude of its input to that threshold. 
     *
     * An optional gain in dB is applied to the output. An optional lookahead
     * (given in ms) allows the limiter to delay its inputs and prepare the
     * envelope in advance.
     */
    class Limiter : public AudioFilter
    {
        public:
            Limiter(float threshold, float gain = 0.0, float lookahead=0.0);

            void set_threshold(float threshold);
            void set_gain(float gain);

        private:
            void filter(std::vector<SAMPLE>& input, std::vector<SAMPLE>& output,
                    unsigned long t);

            /* Coefficients
             */ 
            unsigned lookahead;
            float threshold;
            float gain;

            /* State tracking
             */
            LevelDetector envelope;
            RingBuffer<SAMPLE> inputs;
    };
}

#endif
