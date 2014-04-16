#ifndef LEVEL_DETECTOR_H
#define LEVEL_DETECTOR_H

#include "audio_generics.h"


namespace ClickTrack
{
    /* The level detector takes in a signal, and outputs an envelope that
     * follows the peaks of the input signal. Though it has defaults, the time
     * constants tau can be changed, and are specified in milliseconds.
     *
     * The level detector uses average rms level
     */
    class LevelDetector
    {
        public:
            LevelDetector(float attack_tau = 1.0, float release_tau = 100.0);

            /* The attack tau is used when the signal is greater than the
             * current envelope, and the release tau is used when it is less
             * than the current envelope. This allows you to quickly peak out,
             * then slowly decay.
             */
            void set_attack_tau(float tau);
            void set_release_tau(float tau);

            /* The level detector must be fed samples to compute the next level
             */
            SAMPLE get_next_level(SAMPLE in);

        private:
            /* Detector coefficients
             */
            float attack_alpha, release_alpha;
            SAMPLE last_level;
    };
}

#endif
