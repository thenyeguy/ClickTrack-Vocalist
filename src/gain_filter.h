#ifndef GAINFILTER_H
#define GAINFILTER_H

#include "audio_generics.h"


namespace ClickTrack
{
    /* The gain filter takes a multiplier coefficient, and multiplies all its
     * inputs by the gain factor, given in decibels
     */
    class GainFilter : public AudioFilter
    {
        public:
            GainFilter(float in_gain, unsigned num_channels = 1);

            /* Specifies the gain of the filter in decibels
             */
            void set_gain(float gain);

            /* The LFO modulates the gain parameter by the specified
             * gain in decibels. Setting the LFO to nullptr will remove any LFO
             * effect
             */
            void set_lfo_input(Channel* input);
            void set_lfo_intensity(float db);

        private:
            void filter(std::vector<SAMPLE>& input,
                    std::vector<SAMPLE>& output, unsigned long t);

            float gain;

            Channel* lfo;
            float lfo_intensity;
    };
}

#endif
