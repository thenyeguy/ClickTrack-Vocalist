#include "equalizer.h"

FourPointEqualizer::FourPointEqualizer()
    : FilterBank(1,1), 
      lowpassStage1(PassFilter::low, 0.0),
      lowpassStage2(PassFilter::low, 0.0), 
      lowshelf(ShelfFilter::low, 0.0, 0.0),
      point1(3), 
      point2(0.0, 1.0, 0.0), 
      point3(0.0, 1.0, 0.0),
      highpassStage1(PassFilter::high, 0.0), 
      highpassStage2(PassFilter::high, 0.0),
      highshelf(ShelfFilter::high, 0.0, 0.0),
      point4(3),
      gainFilter(1.0),
      input_channel(NULL)
{
    // Connect stage 1 cascde and multiplexer
    lowpassStage2.set_input_channel(lowpassStage1.get_output_channel());
    point1.set_input_channel(lowpassStage1.get_output_channel(), 0);
    point1.set_input_channel(lowpassStage2.get_output_channel(), 1);
    point1.set_input_channel(lowshelf.get_output_channel(), 2);
    point1.select_channel(2); // Shelf filter will be flat

    // Connect stage 2 and 3
    point2.set_input_channel(point1.get_output_channel());
    point3.set_input_channel(point2.get_output_channel());

    // Connect stage 4 cascade and multiplexer
    highpassStage1.set_input_channel(point3.get_output_channel());
    highpassStage2.set_input_channel(highpassStage1.get_output_channel());
    highshelf.set_input_channel(point3.get_output_channel());
    point4.set_input_channel(highpassStage1.get_output_channel(), 0);
    point4.set_input_channel(highpassStage2.get_output_channel(), 1);
    point4.set_input_channel(highshelf.get_output_channel(), 2);
    point4.select_channel(2); // Shelf filter will be flat

    // Add our output channel
    gainFilter.set_input_channel(point4.get_output_channel());
    output_channels.push_back(gainFilter.get_output_channel());
}


void FourPointEqualizer::set_gain(float gain)
{
    gainFilter.set_gain(gain);
}


void FourPointEqualizer::set_input_channel(Channel* channel, unsigned channel_i)
{
    // Ignore channel_i - this is mono
    lowpassStage1.set_input_channel(channel);
    lowshelf.set_input_channel(channel);
    input_channel = channel;
}


void FourPointEqualizer::remove_channel(unsigned channel_i)
{
    // Ignore channel - this is mono
    lowpassStage1.set_input_channel(NULL);
    lowshelf.set_input_channel(NULL);
    input_channel = NULL;
}


unsigned FourPointEqualizer::get_channel_index(Channel* channel)
{
    if(input_channel == channel)
        return 0;
    else
        throw ChannelNotFound();
}


void FourPointEqualizer::setPoint1(EQFilterMode mode, float cutoff, float gain)
{
    // First select the channel from the mux
    switch(mode)
    {
        case PASS12:
            point1.select_channel(0);
            break;
        case PASS24:
            point1.select_channel(1);
            break;
        case SHELF:
            point1.select_channel(2);
    }

    // Then set our filters
    lowpassStage1.set_cutoff(PassFilter::low, cutoff);
    lowpassStage2.set_cutoff(PassFilter::low, cutoff);
    lowshelf.set_cutoff(ShelfFilter::low, cutoff);
}


void FourPointEqualizer::setPoint4(EQFilterMode mode, float cutoff, float gain)
{
    // First select the channel from the mux
    switch(mode)
    {
        case PASS12:
            point4.select_channel(0);
            break;
        case PASS24:
            point4.select_channel(1);
            break;
        case SHELF:
            point4.select_channel(2);
    }

    // Then set our filters
    highpassStage1.set_cutoff(PassFilter::high, cutoff);
    highpassStage2.set_cutoff(PassFilter::high, cutoff);
    highshelf.set_cutoff(ShelfFilter::high, cutoff);
}


void FourPointEqualizer::setPoint2(float cutoff, float Q, float gain)
{
    point2.set_cutoff(cutoff);
    point2.set_Q(Q);
    point2.set_gain(gain);
}


void FourPointEqualizer::setPoint3(float cutoff, float Q, float gain)
{
    point3.set_cutoff(cutoff);
    point3.set_Q(Q);
    point3.set_gain(gain);
}
