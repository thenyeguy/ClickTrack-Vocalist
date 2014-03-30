#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H

#include "audio_generics.h"


namespace ClickTrack
{
    /* The multiplexer chooses one of its inputs as the output, and throws away
     * all the others. It is nessecary to choose between multiple elements while
     * still maintaining correct internal timing.
     */
    class Multiplexer : public AudioFilter
    {
        public:
            Multiplexer(unsigned num_input_channels);

            /* Select the channel the multiplexer is drawing from. Defaults to
             * channel 0
             */
            void select_channel(unsigned channel);

        private:
            void filter(std::vector<SAMPLE>& input,
                    std::vector<SAMPLE>& output, unsigned long t);
            
            unsigned channel;
    };
}

#endif
