#ifndef ADDER_H
#define ADDER_H

#include "audio_generics.h"


namespace ClickTrack
{
    /* The adder takes in a set of inputs and returns a single output that is
     * the sum of all its inputs
     */
    class Adder : public AudioFilter
    {
        public:
            Adder(unsigned num_input_channels);

        private:
            void filter(std::vector<SAMPLE>& input,
                    std::vector<SAMPLE>& output, unsigned long t);
    };
}

#endif
