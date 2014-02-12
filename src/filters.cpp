#include <cmath>
#include "filters.h"

using namespace ClickTrack;


PassFilter::PassFilter(PassFilterMode in_mode, float in_cutoff,
        unsigned num_channels)
    : AudioFilter(num_channels, num_channels), mode(in_mode),
      cutoff(in_cutoff), x_last(num_channels), y1_last(num_channels)
{
    calculate_coefficients();
}


void PassFilter::set_cutoff(PassFilterMode in_mode, float in_cutoff)
{
    mode = in_mode;
    cutoff = in_cutoff;
    calculate_coefficients();
}


void PassFilter::calculate_coefficients()
{
    a = (tan(M_PI*cutoff/SAMPLE_RATE) - 1) /
        (tan(M_PI*cutoff/SAMPLE_RATE) + 1);
}


void PassFilter::filter(std::vector< std::vector<SAMPLE> >& input,
        std::vector< std::vector<SAMPLE> >& output)
{
    for(int i = 0; i < BLOCK_SIZE; i++)
    {
        for(int j = 0; j < num_input_channels; j++)
        {
            // Calculate this time step
            float x  = input[j][i];
            float y1 = a*x + x_last[j] - a*y1_last[j];
            float y;
            switch(mode)
            {
                case low:
                    y = (x+y1)/2;
                    break;
                case high:
                    y = (x-y1)/2;
                    break;

            }

            // Store the results to our buffers
            x_last[j] = x;
            y1_last[j] = y1;
            output[j][i] = y;
        }
    }
}




ShelfFilter::ShelfFilter(ShelfFilterMode in_mode, float in_cutoff, float in_gain,
        unsigned num_channels)
    : AudioFilter(num_channels, num_channels), mode(in_mode),
      cutoff(in_cutoff), gain(in_gain), x_last(num_channels), y1_last(num_channels)
{
    calculate_coefficients();
}


void ShelfFilter::set_cutoff(ShelfFilterMode in_mode, float in_cutoff)
{
    mode = in_mode;
    cutoff = in_cutoff;
    calculate_coefficients();
}


void ShelfFilter::set_gain(float in_gain)
{
    gain = in_gain;
    calculate_coefficients();
}


void ShelfFilter::calculate_coefficients()
{
    V0 = pow(10,gain/20);
    H0 = V0-1;

    // The a coefficient depends on the mode of operation
    switch(mode)
    {
        case low:
        {
            if(gain < 0) //cut
                a = (tan(M_PI*cutoff/SAMPLE_RATE) - V0) /
                    (tan(M_PI*cutoff/SAMPLE_RATE) + V0);
            else            // boost
                a = (tan(M_PI*cutoff/SAMPLE_RATE) - 1) /
                    (tan(M_PI*cutoff/SAMPLE_RATE) + 1);
            break;
        }

        case high:
        {
            if(gain < 0) //cut
                a = (tan(M_PI*cutoff/SAMPLE_RATE) - 1) /
                    (tan(M_PI*cutoff/SAMPLE_RATE) + 1);
            else            // boost
                a = (V0*tan(M_PI*cutoff/SAMPLE_RATE) - 1) /
                    (V0*tan(M_PI*cutoff/SAMPLE_RATE) + 1);
            break;
        }
    }
}


void ShelfFilter::filter(std::vector< std::vector<SAMPLE> >& input,
        std::vector< std::vector<SAMPLE> >& output)
{
    for(int i = 0; i < BLOCK_SIZE; i++)
    {
        for(int j = 0; j < num_input_channels; j++)
        {
            // Calculate this time step
            float x  = input[j][i];
            float y1 = a*x + x_last[j] - a*y1_last[j];
            float y;
            switch(mode)
            {
                case low:
                    y = H0/2*(x+y1) + x;
                    break;
                case high:
                    y = H0/2*(x-y1) + x;
                    break;

            }

            // Store the results to our buffers
            x_last[j] = x;
            y1_last[j] = y1;
            output[j][i] = y;
        }
    }
}




PeakFilter::PeakFilter(float in_cutoff, float in_Q, float in_gain,
        unsigned num_channels)
    : AudioFilter(num_channels, num_channels), cutoff(in_cutoff),
      Q(in_Q), gain(in_gain),
      x_last1(num_channels), x_last2(num_channels),
      y1_last1(num_channels), y1_last2(num_channels)
{
    calculate_coefficients();
}


void PeakFilter::set_cutoff(float in_cutoff)
{
    cutoff = in_cutoff;
    calculate_coefficients();
}


void PeakFilter::set_Q(float in_Q)
{
    Q = in_Q;
    calculate_coefficients();
}


void PeakFilter::set_gain(float in_gain)
{
    gain = in_gain;
    calculate_coefficients();
}


void PeakFilter::calculate_coefficients()
{
    V0 = pow(10,gain/20);
    H0 = V0-1;

    d = -cos(2*M_PI*cutoff/SAMPLE_RATE);

    float fs = cutoff / Q;
    if(gain < 0) //cut
        a = (tan(M_PI*fs/SAMPLE_RATE) - V0) /
            (tan(M_PI*fs/SAMPLE_RATE) + V0);
    else            // boost
        a = (tan(M_PI*fs/SAMPLE_RATE) - 1) /
            (tan(M_PI*fs/SAMPLE_RATE) + 1);
}


void PeakFilter::filter(std::vector< std::vector<SAMPLE> >& input,
        std::vector< std::vector<SAMPLE> >& output)
{
    for(int i = 0; i < BLOCK_SIZE; i++)
    {
        for(int j = 0; j < num_input_channels; j++)
        {
            // Calculate this time step
            float x  = input[j][i];
            float y1 = -a*x + d*(1-a)*x_last1[j] + x_last2[j]
                     - d*(1-a)*y1_last1[j] + a*y1_last2[j];
            float y = H0/2*(x-y1) + x;


            // Store the results to our buffers
            x_last2[j] = x_last1[j];
            x_last1[j] = x;

            y1_last2[j] = y1_last1[j];
            y1_last1[j] = y1;

            output[j][i] = y;
        }
    }
}
