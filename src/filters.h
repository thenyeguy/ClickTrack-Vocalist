#ifndef FILTERS_H
#define FILTERS_H

#include <vector>
#include "filter_generics.h"

using namespace std;
using namespace FilterGenerics;


namespace Filters
{
    /* A shelf filter is a filter that can either cut or boost frequencies,
     * below of above a cutoff frequency. Essentially, they operate as one of:
     *      lowpass, lowcut, highpass, highcut
     *
     * Implements filters as described in:
     *      http://www.music.mcgill.ca/~ich/classes/FiltersChap2.pdf
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

        private:
            void filter(SAMPLE** input, SAMPLE** output);

            /* Specifies the filter coefficients
             */
            ShelfFilterMode mode;
            float H0, V0, a;
             
            /* Previous computation results. Used to implement a single pole in
             * the filters
             */
            vector<float> x_last, y_last, y1_last;
    };
}

#endif
