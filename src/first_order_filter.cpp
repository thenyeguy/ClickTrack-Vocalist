#include <cmath>
#include "first_order_filter.h"

using namespace ClickTrack;


FirstOrderFilter::FirstOrderFilter(Mode in_mode, float in_cutoff, float in_gain,
        unsigned num_channels)
    : AudioFilter(num_channels, num_channels), mode(in_mode), cutoff(in_cutoff),
      gain(in_gain), x_last(num_channels), y1_last(num_channels)
{
    calculate_coefficients();
}


void FirstOrderFilter::set_mode(Mode in_mode)
{
    mode = in_mode;
    calculate_coefficients();
}


void FirstOrderFilter::set_cutoff(float in_cutoff)
{
    cutoff = in_cutoff;
    calculate_coefficients();
}


void FirstOrderFilter::set_gain(float in_gain)
{
    gain = in_gain;
    calculate_coefficients();
}


void FirstOrderFilter::calculate_coefficients()
{
    // Params for the shelf filter
    V0 = pow(10,gain/20);
    H0 = V0-1;
    float K = tan(M_PI*cutoff/SAMPLE_RATE);

    switch(mode)
    {
        case LOWPASS:
        case HIGHPASS:
            a = (K - 1) / (K + 1);
            break;

        case LOWSHELF:
            if(gain < 0) //cut
                a = (K - V0) / (K + V0);
            else         // boost
                a = (K - 1) / (K + 1);
            break;

        case HIGHSHELF:
            if(gain < 0) //cut
                a = (V0*K - 1) / (K + 1);
            else         // boost
                a = (K - 1) / (K + 1);
            break;
    }
}


void FirstOrderFilter::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t)
{
    for(int i = 0; i < input.size(); i++)
    {
        // Calculate this time step
        float x  = input[i];
        float y1 = a*x + x_last[i] - a*y1_last[i];
        float y;
        switch(mode)
        {
            case LOWPASS:
                y = (x+y1)/2;
                break;
            case HIGHPASS:
                y = (x-y1)/2;
                break;
            case LOWSHELF:
                y = H0/2*(x+y1) + x;
                break;
            case HIGHSHELF:
                y = H0/2*(x-y1) + x;
                break;
        }

        // Store the results to our buffers
        x_last[i] = x;
        y1_last[i] = y1;
        output[i] = y;
    }
}
