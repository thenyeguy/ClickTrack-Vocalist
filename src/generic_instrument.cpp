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
    return output_channels[channel];
}

const unsigned GenericInstrument::get_num_output_channels()
{
    return output_channels.size();
}
