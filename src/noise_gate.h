#ifndef NOISE_GATE_H
#define NOISE_GATE_H

#include "level_detector.h"
#include "ringbuffer.h"


namespace ClickTrack
{
    /* The noise gate mutes a signal below a certain threshold. It opens at the
     * on threshold, and stays open until it drops below the off threshold.
     *
     * An optional gain in dB is applied to the output. An optional lookahead
     * (given in ms) allows the limiter to delay its inputs and prepare the
     * envelope in advance.
     */
    class NoiseGate : public AudioFilter
    {
        public:
            NoiseGate(float on_threshold, float off_threshold, float gain=0.0,
                    float lookahead=0.0);

            void set_on_threshold(float threshold);
            void set_off_threshold(float threshold);
            void set_gain(float gain);

        private:
            void filter(std::vector<SAMPLE>& input, std::vector<SAMPLE>& output,
                    unsigned long t);

            /* Coefficients
             */ 
            unsigned lookahead;
            float on_threshold, off_threshold;
            float gain;

            /* State tracking
             */
            bool active;
            LevelDetector envelope;
            RingBuffer<SAMPLE> inputs;
    };
}

#endif
