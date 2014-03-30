#ifndef SPEAKER_H
#define SPEAKER_H

#include "audio_generics.h"
#include "portaudio_wrapper.h"


namespace ClickTrack
{
    /* The speaker is an output device. It uses the default output device on
     * your computer, and pushes its data out to portaudio.
     */
    class Speaker : public AudioConsumer
    {
        public:
            Speaker(unsigned num_inputs = 1, bool defaultDevice=true);

            /* This callback is called whenever we write out to the stream. It
             * passes the starting time of next the buffer to be filled, and the
             * void* payload provided.
             */
            typedef void (*callback_t)(unsigned long time, void* payload);
            void register_callback(callback_t callback, void* payload);

        private:
            void process_inputs(std::vector<SAMPLE>& input, unsigned long t);

            /* Store our stream results
             */
            std::vector< std::vector<SAMPLE> > buffer;
            OutputStream stream;

            /* The callback function
             */
            callback_t callback;
            void* payload;
    };
}


#endif
