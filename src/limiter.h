#ifndef LIMITER_H
#define LIMITER_H

#include "level_detector.h"
#include "ringbuffer.h"


namespace ClickTrack
{
    /* The limiter takes in a threshold in decibels, and then limits the
     * amplitude of its input to that threshold. An optional lookahead (given in
     * ms) allows the limiter to delay its inputs and prepare the envelope in
     * advance.
     */
    class Limiter : public AudioFilter
    {
        public:
            Limiter(float threshold, float lookahead=0.0);

            void set_threshold(float threshold);

        private:
            void filter(std::vector<SAMPLE>& input, std::vector<SAMPLE>& output,
                    unsigned long t);

            /* Coefficients
             */ 
            unsigned lookahead;
            float threshold;

            /* State tracking
             */
            LevelDetector envelope;
            RingBuffer<SAMPLE> inputs;
    };
}

#endif
