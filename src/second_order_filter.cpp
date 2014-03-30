#include <cmath>
#include "second_order_filter.h"

using namespace ClickTrack;


SecondOrderFilter::SecondOrderFilter(Mode in_mode, float in_cutoff, 
        float in_gain, float in_Q, unsigned num_channels)
    : AudioFilter(num_channels, num_channels), mode(in_mode), cutoff(in_cutoff),
      Q(in_Q), gain(in_gain), x_last1(num_channels), x_last2(num_channels),
      y_last1(num_channels), y_last2(num_channels)
{
    calculate_coefficients();
}


void SecondOrderFilter::set_mode(Mode in_mode)
{
    mode = in_mode;
    calculate_coefficients();
}


void SecondOrderFilter::set_cutoff(float in_cutoff)
{
    cutoff = in_cutoff;
    calculate_coefficients();
}


void SecondOrderFilter::set_gain(float in_gain)
{
    gain = in_gain;
    calculate_coefficients();
}


void SecondOrderFilter::set_Q(float in_Q)
{
    Q = in_Q;
    calculate_coefficients();
}


void SecondOrderFilter::calculate_coefficients()
{
    // Constants used in coefficients calculation
    float K = tan(M_PI*cutoff/SAMPLE_RATE);
    float V0 = pow(10,gain/20);

    switch(mode)
    {
        case LOWPASS:
        case HIGHPASS:
            b0 = 1/(1 + sqrt(2)*K + K*K);
            if(mode == LOWPASS) b0 *= K*K;

            b1 = -2/(1 + sqrt(2)*K + K*K);
            if(mode == LOWPASS) b1 *= -1*K*K;

            b2 = 1/(1 + sqrt(2)*K + K*K);
            if(mode == LOWPASS) b2 *= K*K;

            a1 = 2*(K*K - 1) / (1 + sqrt(2)*K + K*K);
            a2 = (1 - sqrt(2)*K + K*K) / (1 + sqrt(2)*K + K*K);

            break;

        case LOWSHELF:
            if(gain < 0) // cut
            {
                V0 = 1/V0;
                b0 = (1 + sqrt(2)*K + K*K) / (1 + sqrt(2*V0)*K + V0*K*K);
                b1 = 2*(K*K - 1) / (1 + sqrt(2*V0)*K + V0*K*K);
                b2 = (1 - sqrt(2)*K + K*K) / (1 + sqrt(2*V0)*K + V0*K*K);
                a1 = 2*(V0*K*K - 1) / (1 + sqrt(2*V0)*K + V0*K*K);
                a2 = (1 - sqrt(2*V0)*K + V0*K*K) / (1 + sqrt(2*V0)*K + V0*K*K);
            }
            else // boost
            {
                b0 = (1 + sqrt(2*V0)*K + V0*K*K) / (1 + sqrt(2)*K + K*K);
                b1 = 2*(V0*K*K - 1) / (1 + sqrt(2)*K + K*K);
                b2 = (1 - sqrt(2*V0)*K + V0*K*K) / (1 + sqrt(2)*K + K*K);
                a1 = 2*(K*K - 1) / (1 + sqrt(2)*K + K*K);
                a2 = (1 - sqrt(2)*K + K*K) / (1 + sqrt(2)*K + K*K);
            }
            break;

        case HIGHSHELF:
            if(gain < 0) // cut
            {
                V0 = 1/V0;
                b0 = (1 + sqrt(2)*K + K*K) / (V0 + sqrt(2*V0)*K + K*K);
                b1 = 2*(K*K - 1) / (V0 + sqrt(2*V0)*K + K*K);
                b2 = (1 - sqrt(2)*K + K*K) / (V0 + sqrt(2*V0)*K + K*K);
                a1 = 2*(K*K/V0 - 1) / (1 + sqrt(2/V0)*K + K*K/V0);
                a2 = (1 - sqrt(2/V0)*K + K*K/V0) / (1 + sqrt(2/V0)*K + K*K/V0);
            }
            else // boost
            {
                b0 = (V0 + sqrt(2*V0)*K + K*K) / (1 + sqrt(2)*K + K*K);
                b1 = 2*(K*K - V0) / (1 + sqrt(2)*K + K*K);
                b2 = (V0 - sqrt(2*V0)*K + K*K) / (1 + sqrt(2)*K + K*K);
                a1 = 2*(K*K - 1) / (1 + sqrt(2)*K + K*K);
                a2 = (1 - sqrt(2)*K + K*K) / (1 + sqrt(2)*K + K*K);
            }
            break;

        case PEAK:
            if(gain < 0) // cut
            {
                V0 = 1/V0;
                b0 = (1 + K/Q + K*K) / (1 + V0*K/Q + K*K);
                b1 = 2*(K*K - 1) / (1 + V0*K/Q + K*K);
                b2 = (1 - K/Q + K*K) / (1 + V0*K/Q + K*K);
                a1 = 2*(K*K - 1) / (1 + V0*K/Q + K*K);
                a2 = (1 - V0*K/Q + K*K) / (1 + V0*K/Q + K*K);
            }
            else // boost
            {
                b0 = (1 + V0*K/Q + K*K) / (1 + K/Q + K*K);
                b1 = 2*(K*K - 1) / (1 + K/Q + K*K);
                b2 = (1 - V0*K/Q + K*K) / (1 + K/Q + K*K);
                a1 = 2*(K*K - 1) / (1 + K/Q + K*K);
                a2 = (1 - K/Q + K*K) / (1 + K/Q + K*K);
            }
            break;
    }
}


void SecondOrderFilter::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t)
{
    for(int i = 0; i < input.size(); i++)
    {
        // Calculate this time step
        float x  = input[i];
        float y = b0*x + b1*x_last1[i] + b2*x_last2[i]
            - a1*y_last1[i] - a2*y_last2[i];

        // Store the results to our buffers
        x_last2[i] = x_last1[i];
        x_last1[i] = x;

        y_last2[i] = y_last1[i];
        y_last1[i] = y;

        output[i] = y;
    }
}
