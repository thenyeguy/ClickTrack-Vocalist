#ifndef FIRST_ORDER_FILTER_H
#define FIRST_ORDER_FILTER_H

#include <vector>
#include "audio_generics.h"


/* Implements a set of rudimentary filters for frequency rejection and
 * equalization. All filters are based on filter designs from:
 *
 *      http://www.music.mcgill.ca/~ich/classes/FiltersChap2.pdf 
 */
namespace ClickTrack
{
    /* First order filters use a first order all pass filter.
     *
     * Lowpass/highpass filter at 6dB per octave, and -3dB at the cutoff, and
     * ignore their gain parameter.
     *
     * Shelf filters set frequencies above/below the specified cutoff to the
     * specified gain.
     *
     * Cutoff is in Hz, gain is in dB
     */
    class FirstOrderFilter : public AudioFilter
    {
        public:
            enum Mode { LOWPASS, LOWSHELF, HIGHPASS, HIGHSHELF };
            FirstOrderFilter(Mode mode, float cutoff, float gain=1.0,
                    unsigned num_channels = 1);

            void set_mode(Mode mode);
            void set_cutoff(float cutoff);
            void set_gain(float gain);

        private:
            void filter(std::vector<SAMPLE>& input,
                    std::vector<SAMPLE>& output, unsigned long t);

            /* Used to recompute coefficients
             */
            void calculate_coefficients();

            /* The filter parameters and coefficients
             */
            Mode mode;
            float cutoff, gain;

            float a;
            float V0, H0;
             
            /* Previous computation results. Used to implement a single pole in
             * the filters
             */
            std::vector<float> x_last, y1_last;
    };
}

#endif
