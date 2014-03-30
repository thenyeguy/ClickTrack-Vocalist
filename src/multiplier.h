#ifndef MULTIPLIER_H
#define MULTIPLIER_H

#include "audio_generics.h"


namespace ClickTrack
{
    /* The multiplier takes in a set of inputs and returns a single output that
     * is the product of all its inputs
     */
    class Multiplier : public AudioFilter
    {
        public:
            Multiplier(unsigned num_input_channels);

        private:
            void filter(std::vector<SAMPLE>& input,
                    std::vector<SAMPLE>& output, unsigned long t);
    };
}

#endif
