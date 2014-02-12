#include "generic_instrument.h"

using namespace ClickTrack;


GenericInstrument::GenericInstrument(int midi_channel)
    : output_channels(), midi(this, midi_channel)
{}


void GenericInstrument::add_output_channel(Channel* channel)
{
    output_channels.push_back(channel);
}


Channel* GenericInstrument::get_output_channel(int channel)
{
    if(channel > output_channels.size())
        throw ChannelOutOfRange();
    return output_channels[channel];
}

const unsigned GenericInstrument::get_num_output_channels()
{
    return output_channels.size();
}
