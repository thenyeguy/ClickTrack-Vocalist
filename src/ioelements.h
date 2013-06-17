#ifndef IOELEMENTS_H
#define IOELEMENTS_H

#include "portaudio_wrapper.h"
#include "filtergenerics.h"


namespace IOElements
{
    class Microphone : public FilterGenerics::AudioGenerator
    {
        private:
            Portaudio::InputStream* streams;
        public:
            Microphone(Portaudio::InputStream* inputs, unsigned num_channels = 1);

            void generate_outputs(SAMPLE** outputs);
    };


    class Speaker : public FilterGenerics::AudioConsumer
    {
        private:
            Portaudio::OutputStream* streams;
        public:
            Speaker(Portaudio::OutputStream* outputs,
                    FilterGenerics::OutputChannel* inputs,
                    unsigned num_inputs = 1);

            void process_inputs(SAMPLE** inputs);
    };
}


#endif
