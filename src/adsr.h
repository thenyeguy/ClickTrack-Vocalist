#ifndef DELAY_H
#define DELAY_H

#include "filter_generics.h"

using namespace FilterGenerics;


namespace Filters
{
    /* This applies an ADSR envelope to its input channels. This filter cannot
     * operate without a controller attached to it, as without being triggered
     * it will remain in silent operation.
     */
    class ADSRFilter : public AudioFilter
    {
        public:
            /* Constructor... times are expressed in seconds */
            ADSRFilter(float attack_time, float decay_time,
                       float sustain_level, float release_time,
                       OutputChannel** in_input_channels,
                       unsigned in_num_channels = 1);

            /* These functions are used to trigger the envelope to begin or end
             */
            void on_note_down();
            void on_note_up();

        private:
            void filter(SAMPLE** input, SAMPLE** output);

            /* ADSR filter is in several states, and transitions in order
             * through these states during its operation. When transitioning to
             * each state, the startTime is set to the trigger time again.
             */
            enum ADSRState { silent, attack, decay, sustain, release };
            ADSRState state;
            unsigned triggerTime;
            unsigned t;

            /* The following "times" are expressed in samples
             */
            unsigned attack_time, decay_time, release_time;
            float sustain_level;
    };
}

#endif
