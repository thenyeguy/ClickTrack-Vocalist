#include "generic_instrument.h"

using namespace FilterGenerics;
using namespace Instruments;


GenericInstrument::GenericInstrument(int midi_channel)
    : output_channels(), midi(this, midi_channel)
{}


GenericInstrument::~GenericInstrument()
{}


void GenericInstrument::add_output_channel(OutputChannel* channel)
{
    output_channels.push_back(channel);
}


FilterGenerics::OutputChannel*
GenericInstrument::get_output_channel(int channel)
{
    if(channel > output_channels.size())
    {
        std::cerr << "The instrument does not have a channel " << channel <<
                     "." << std::endl;
    }
    return output_channels[channel];
}

const unsigned GenericInstrument::get_num_output_channels()
{
    return output_channels.size();
}
