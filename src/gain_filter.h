#ifndef GAINFILTER_H
#define GAINFILTER_H

#include "audio_generics.h"


namespace ClickTrack
{
    /* The gain filter takes a multiplier coefficient, and multiplies all its
     * inputs by the gain factor.
     */
    class GainFilter : public AudioFilter
    {
        public:
            GainFilter(float in_gain, unsigned num_channels = 1);

            void set_gain(float gain);

        private:
            void filter(std::vector<SAMPLE>& input,
                    std::vector<SAMPLE>& output, unsigned long t);

            float gain;
    };
}

#endif
