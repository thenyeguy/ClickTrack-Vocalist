#ifndef ELEMENTARYFILTERS_H
#define ELEMENTARYFILTERS_H

#include <vector>
#include "filter_generics.h"
#include "ringbuffer.h"

using namespace FilterGenerics;


namespace Filters
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
            void filter(std::vector< std::vector<SAMPLE> >& input,
                    std::vector< std::vector<SAMPLE> >& output);

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
            void filter(std::vector< std::vector<SAMPLE> >& input, 
                    std::vector< std::vector<SAMPLE> >& output);
    };


    /* The multiplier takes in a set of inputs and returns a single output that
     * is the product of all its inputs
     */
    class Multiplier : public AudioFilter
    {
        public:
            Multiplier(unsigned num_input_channels);

        private:
            void filter(std::vector< std::vector<SAMPLE> >& input, 
                    std::vector< std::vector<SAMPLE> >& output);
    };


    /* The clip detector monitors a channel for clipping, and will print to the
     * console if it occurs. Rate limited, and acts as a pass through.
     */
    class ClipDetector : public AudioFilter
    {
        public:
            ClipDetector(float rate, unsigned num_channels=1);
            
        private:
            void filter(std::vector< std::vector<SAMPLE> >& input,
                    std::vector< std::vector<SAMPLE> >& output);

            /* Measures "time" in samples
             */
            const unsigned rate;
            unsigned next_time;
    };
}

#endif
