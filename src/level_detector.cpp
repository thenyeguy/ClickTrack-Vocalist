#include <cmath>
#include "level_detector.h"

using namespace ClickTrack;


LevelDetector::LevelDetector(float attack_tau, float release_tau)
{
    set_attack_tau(attack_tau);
    set_release_tau(release_tau);

    last_level = 0.0;
}


void LevelDetector::set_attack_tau(float attack_tau)
{
    attack_alpha = exp(-1.0 / (SAMPLE_RATE * attack_tau / 1000));
}


void LevelDetector::set_release_tau(float release_tau)
{
    release_alpha = exp(-1.0 / (SAMPLE_RATE * release_tau / 1000));
}


SAMPLE LevelDetector::get_next_level(SAMPLE in)
{
    // Square the input
    in *= in;

    // Leaky integrate using the correct time constant
    if(in > last_level)
        last_level = attack_alpha*last_level + (1-attack_alpha)*in;
    else
        last_level = release_alpha*last_level + (1-release_alpha)*in;

    return sqrt(last_level);
}
