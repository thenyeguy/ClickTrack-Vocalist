#include <cmath>
#include <iostream>
#include "clip_detector.h"

using namespace ClickTrack;


ClipDetector::ClipDetector(float in_rate, unsigned num_channels)
    : AudioFilter(num_channels), rate(in_rate*44100), next_time(0)
{}

void ClipDetector::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t)
{
    for(int i = 0; i < input.size(); i++)
    {
        if(fabs(input[i]) >= 1.0 && t > next_time)
        {
            std::cout << "AUDIO CLIPPING DETECTED" << std::endl;
            next_time = t + rate;
        }

        // Copy inputs to outpts
        output[i] = input[i];
    }
}
