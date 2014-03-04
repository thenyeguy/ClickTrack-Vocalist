#ifndef EQUALIZER_H
#define EQUALIZER_H

#include "audio_generics.h"
#include "basic_elements.h"
#include "filters.h"


namespace ClickTrack
{
    /* A full mono equalizer plugin with four points. They are as follows:
     *
     *     Point 1: Either a lowpass(12dB or 24dB/octave) or a low shelf
     *     Points 2&3: Peak filters with adjustable Q
     *     Point 4: Either a highpass(12dB or 24dB/octae) or a high shelf
     *
     * The equilizer is initialized to a flat spectrum. Each point can then be
     * set one at a time using the setters.
     */
    class FourPointEqualizer : public FilterBank
    {
        public:
            FourPointEqualizer();

            void set_input_channel(Channel* channel, unsigned channel_i = 0);
            void remove_channel(unsigned channel_i = 0);
            unsigned get_channel_index(Channel* channel);

            /* Sets the global gain of the EQ
             */
            void set_gain(float gain);

            /* Points 1 and 4 have the same format, as they are similar filters.
             * If a pass filter is selected, the gain is ignored
             */
            enum EQFilterMode { PASS12, PASS24, SHELF };
            void setPoint1(EQFilterMode mode, float cutoff, float gain);
            void setPoint4(EQFilterMode mode, float cutoff, float gain);

            /* Points 2 and 3 are simple peak filters
             */
            void setPoint2(float cutoff, float Q, float gain);
            void setPoint3(float cutoff, float Q, float gain);

        private:
            /* Signal chain for each equalizer stage.
             * The lowpass/highpass filters have two stages - one for
             * 12dB/octave, one cascaded for 24dB/octave.
             */
            PassFilter lowpassStage1;
            PassFilter lowpassStage2;
            ShelfFilter lowshelf;
            Multiplexer point1;

            PeakFilter point2;
            PeakFilter point3;

            PassFilter highpassStage1;
            PassFilter highpassStage2;
            ShelfFilter highshelf;
            Multiplexer point4;

            GainFilter gainFilter;

            /* The current input channel
             */
            Channel* input_channel;
    };
}

#endif
