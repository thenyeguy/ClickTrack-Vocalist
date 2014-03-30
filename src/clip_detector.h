#ifndef CLIP_DETECTOR_H
#define CLIP_DETECTOR_H

#include "audio_generics.h"


namespace ClickTrack
{
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
