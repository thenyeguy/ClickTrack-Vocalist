#ifndef BASICELEMENTS_H
#define BASICELEMENTS_H

#include <vector>
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


    /* The clip detector monitors a channel for clipping, and will print to the
     * console if it occurs. Rate limited, and acts as a pass through.
     */
    class ClipDetector : public AudioFilter
    {
        public:
            ClipDetector(float rate, unsigned num_channels=1);
            
        private:
            void filter(std::vector<SAMPLE>& input,
                    std::vector<SAMPLE>& output, unsigned long t);

            /* Measures "time" in samples
             */
            const unsigned rate;
            unsigned next_time;
    };
}

#endif
