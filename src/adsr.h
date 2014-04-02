#ifndef ADSR_H
#define ADSR_H

#include "audio_generics.h"
#include "scheduler.h"


namespace ClickTrack
{
    /* This applies an ADSR envelope to its input channels. This filter cannot
     * operate without a controller attached to it, as without being triggered
     * it will remain in silent operation.
     */
    class ADSRFilter : public AudioFilter
    {
        public:
            /* Constructor. Times are expressed in seconds, gain in decibels */
            ADSRFilter(float attack_time=.005, float decay_time=.1,
                       float sustain_level=.5, float release_time=.1,
                       float gain=0.0, unsigned num_channels=1);

            /* These functions are used to trigger the envelope to begin or end
             */
            void on_note_down(unsigned long time=0);
            void on_note_up(unsigned long time=0);

            static void note_down_callback(ADSRFilter& caller, void* payload);
            static void note_up_callback(ADSRFilter& caller, void* payload);

            /* Setters for parameters
             */
            void set_attack_time(float attack_time);
            void set_decay_time(float decay_time);
            void set_sustain_level(float sustain_level);
            void set_release_time(float release_time);
            void set_gain(float gain);

        private:
            void filter(std::vector<SAMPLE>& input,
                    std::vector<SAMPLE>& output, unsigned long t);

            /* Used to schedule note events
             */
            FunctionScheduler<ADSRFilter> scheduler;

            /* ADSR filter is in several states, and transitions in order
             * through these states during its operation. When transitioning to
             * each state, the startTime is set to the trigger time again.
             */
            enum ADSRState { silent, attack, decay, sustain, release };
            ADSRState state;

            unsigned trigger_time;
            unsigned end_time;

            /* Internally, we adjust the multiplier by a constant every time
             * step. The multiplier is reset based on its target when we change
             * state
             */
            float multiplier;
            float delta_mult;

            /* The following "times" are expressed in samples
             */
            unsigned attack_time, decay_time, release_time;
            float sustain_level;
            float gain;
    };
}

#endif
