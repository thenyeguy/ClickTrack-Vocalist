#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "audio_generics.h"
#include "portaudio_wrapper.h"


namespace ClickTrack
{
    /* The microphone is an input device. It uses the default input device on
     * your computer, and pulls its data from portaudio.
     */
    class Microphone : public AudioGenerator
    {
        public:
            Microphone(unsigned num_channels = 1, bool defaultDevice=true);

        private:
            void generate_outputs(std::vector<SAMPLE>& output, unsigned long t);

            /* Store our stream results
             */
            std::vector< std::vector<SAMPLE> > buffer;
            InputStream stream;
    };
}


#endif
