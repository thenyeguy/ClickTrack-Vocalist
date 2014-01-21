#include <cmath>
#include "filters.h"

using namespace FilterGenerics;
using namespace Filters;


ShelfFilter::ShelfFilter(ShelfFilterMode in_mode, float in_cutoff, float in_gain,
    OutputChannel** in_input_channels, unsigned in_num_channels)
    : AudioFilter(in_num_channels, in_num_channels, in_input_channels),
      x_last(in_num_channels, 0.0), y1_last(in_num_channels, 0.0)
{
    // Initialize coefficients
    mode = in_mode;
    V0 = pow(10,in_gain/20);
    H0 = V0-1;

    // The a coefficient depends on the mode of operation
    switch(mode)
    {
        case low:
        {
            if(in_gain < 0) //cut
                a = (tan(M_PI*in_cutoff/Portaudio::DEFAULT_SAMPLE_RATE) - V0) /
                    (tan(M_PI*in_cutoff/Portaudio::DEFAULT_SAMPLE_RATE) + V0);
            else            // boost
                a = (tan(M_PI*in_cutoff/Portaudio::DEFAULT_SAMPLE_RATE) - 1) /
                    (tan(M_PI*in_cutoff/Portaudio::DEFAULT_SAMPLE_RATE) + 1);
            break;
        }

        case high:
        {
            if(in_gain < 0) //cut
                a = (tan(M_PI*in_cutoff/Portaudio::DEFAULT_SAMPLE_RATE) - 1) /
                    (tan(M_PI*in_cutoff/Portaudio::DEFAULT_SAMPLE_RATE) + 1);
            else            // boost
                a = (V0*tan(M_PI*in_cutoff/Portaudio::DEFAULT_SAMPLE_RATE) - 1) /
                    (V0*tan(M_PI*in_cutoff/Portaudio::DEFAULT_SAMPLE_RATE) + 1);
            break;
        }
    }

    cout << "Mode=" << mode << ", V0=" << V0 << ", H0=" << H0 <<
            ", a=" << a << endl;
}


void ShelfFilter::filter(SAMPLE** input, SAMPLE** output)
{
    for(int i = 0; i < DEFAULT_BLOCK_SIZE; i++)
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
            y1_last[j] = y;
            output[j][i] = y;
        }
    }
}
