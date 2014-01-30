#ifndef FILTERS_H
#define FILTERS_H

#include <vector>
#include "filter_generics.h"

using namespace std;
using namespace FilterGenerics;


/* Implements a set of rudimentary filters for frequency rejection and
 * equalization. All filters are based on filter designs from:
 *
 *      http://www.music.mcgill.ca/~ich/classes/FiltersChap2.pdf 
 */
namespace Filters
{
    /* A pass filter is a filter that attenuates all frequencies above or below
     * a cutoff frequency.
     *
     * To initialize, one must simply specify whether the filter is highpass
     * or lowpass, the cutoff in Hz, and the gain in decibels.
     */
    class PassFilter : public AudioFilter
    {
        public:
            enum PassFilterMode { low, high };
            PassFilter(PassFilterMode mode, float cutoff,
                        OutputChannel** in_input_channels,
                        unsigned in_num_channels = 1);

            void set_cutoff(PassFilterMode mode, float cutoff);

        private:
            void filter(SAMPLE** input, SAMPLE** output);

            /* Specifies the filter coefficients
             */
            void calculate_coefficients();
            PassFilterMode mode;
            float cutoff;

            float a;
             
            /* Previous computation results. Used to implement a single pole in
             * the filters
             */
            vector<float> x_last, y1_last;
    };


    /* A shelf filter is a filter that can either cut or boost frequencies,
     * below of above a cutoff frequency.
     *
     * To initialize, one must simply specify whether the shelf is high (above
     * the cutoff), or low (below the cutoff), the cutoff in Hz, and the gain in
     * decibels.
     */
    class ShelfFilter : public AudioFilter
    {
        public:
            enum ShelfFilterMode { low, high };
            ShelfFilter(ShelfFilterMode mode, float cutoff, float gain,
                        OutputChannel** in_input_channels,
                        unsigned in_num_channels = 1);

            void set_cutoff(ShelfFilterMode mode, float cutoff);
            void set_gain(float in_gain);

        private:
            void filter(SAMPLE** input, SAMPLE** output);

            /* Specifies the filter coefficients
             */
            void calculate_coefficients();
            
            ShelfFilterMode mode;
            float cutoff, gain;

            float H0, V0, a;
             
            /* Previous computation results. Used to implement a single pole in
             * the filters
             */
            vector<float> x_last, y1_last;
    };


    /* A peak filter is a filter that can either cut or boost frequencies,
     * centered around a cutoff frequency.
     *
     * To initialize, one must simply specify the cutoff in Hz, the filter Q,
     * and the gain in decibels.
     */
    class PeakFilter : public AudioFilter
    {
        public:
            PeakFilter(float cutoff, float Q, float gain,
                        OutputChannel** in_input_channels,
                        unsigned in_num_channels = 1);

            void set_cutoff(float cutoff);
            void set_Q(float Q);
            void set_gain(float gain);

        private:
            void filter(SAMPLE** input, SAMPLE** output);

            /* Specifies the filter coefficients
             */
            void calculate_coefficients();
            float cutoff, Q, gain;

            float H0, V0, d, a;
             
            /* Previous computation results. Used to implement a single pole in
             * the filters
             */
            vector<float> x_last1, x_last2, y1_last1, y1_last2;
    };
}

#endif
