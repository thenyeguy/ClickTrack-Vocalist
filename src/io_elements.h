#ifndef IOELEMENTS_H
#define IOELEMENTS_H

#include "portaudio_wrapper.h"
#include "filter_generics.h"


namespace IOElements
{
    /* The microphone is an input device. It uses the default input device on
     * your computer, and pulls its data from portaudio.
     */
    class Microphone : public FilterGenerics::AudioGenerator
    {
        private:
            Portaudio::InputStream* streams;
        public:
            Microphone(Portaudio::InputStream* inputs, unsigned num_channels = 1);

            void generate_outputs(SAMPLE** outputs);
    };


    /* The speaker is an output device. It uses the default output device on
     * your computer, and pushes its data out to portaudio.
     */
    class Speaker : public FilterGenerics::AudioConsumer
    {
        private:
            Portaudio::OutputStream* streams;
        public:
            Speaker(Portaudio::OutputStream* outputs,
                    FilterGenerics::OutputChannel** inputs,
                    unsigned num_inputs = 1);

            void process_inputs(SAMPLE** inputs);
    };
}


#endif
