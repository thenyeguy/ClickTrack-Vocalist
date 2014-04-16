#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "level_detector.h"
#include "ringbuffer.h"


namespace ClickTrack
{
    /* The compression takes in a threshold in decibels, and a ratio that
     * specifies the ratio of attenuation above the threshold. For example,
     * a compression ratio of 0 provides NO attenuation. A compression ratio of
     * attenuates down the the
     *
     * An optional gain in dB is applied to the output. An optional lookahead
     * (given in ms) allows the compressor to delay its inputs and prepare the
     * envelope in advance.
     */
    class Compressor : public AudioFilter
    {
        public:
            Compressor(float threshold, float compression_ratio,
                    float gain=0.0, float lookahead=0.0);

            void set_threshold(float threshold);
            void set_compression_ratio(float compression_ratio);
            void set_gain(float gain);

        private:
            void filter(std::vector<SAMPLE>& input, std::vector<SAMPLE>& output,
                    unsigned long t);

            /* Coefficients
             */ 
            unsigned lookahead;
            float threshold;
            float compression_ratio;
            float gain;

            /* State tracking
             */
            LevelDetector envelope;
            RingBuffer<SAMPLE> inputs;
    };
}

#endif
