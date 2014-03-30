#include <cmath>
#include "wav_reader.h"
#include "reverb.h"

using namespace ClickTrack;


MoorerReverb::MoorerReverb(Room in_room, float in_rev_time, float in_gain, 
        float in_wetness, unsigned num_channels)
    : AudioFilter(num_channels, num_channels), room(in_room), 
      rev_time(in_rev_time), gain(in_gain), wetness(in_wetness),
      tapped_delay_lines(), comb_delay_lines(), tapped_delay_line_outputs(), 
      comb_outputs()
{
    // Get room parameters 
    switch(room)
    {
        case HALL:
            // For these parameters, see pg. 24 from Moorer
            tapped_delays = {190, 948, 992, 1182, 1191, 1314, 2020, 2523, 2589,
                2624, 2699, 3118, 3122, 3202, 3268, 3321, 3515};
            tapped_gains = {.841, .504, .491, .379, .380, .346, .289, .272,
                192, .193, .217, .181, .180, .181, .176, .142, .167, .134};

            // For these parameters, see pg. 18 from Moorer
            comb_delays = {2205, 2470, 2690, 2999, 3175, 3440};

            break;
    }

    // Set output delays so last FIR exit corresponds to first exit from the
    // comb filter. Set lowest to one to avoid breaking
    tapped_out_delay = 0;
    comb_out_delay = tapped_delays.back();

    // Set the comb filter gains
    set_comb_filter_gains();

    // Allocate the ringbuffers
    allocate_ringbuffers();
}


void MoorerReverb::set_rev_time(float in_rev_time)
{
    rev_time = in_rev_time;
    set_comb_filter_gains(); // reset gains to get the correct reverb time
}


void MoorerReverb::set_gain(float in_gain)
{
    gain = in_gain;
}


void MoorerReverb::set_wetness(float in_wetness)
{
    wetness = in_wetness;
}


void MoorerReverb::set_comb_filter_gains()
{
    // Set comb filter gains for a certain reverberation time
    for(unsigned i = 0; i < comb_delays.size(); i++)
    {
        comb_gains.push_back(pow(10, 
                    -3.0 * comb_delays[i]/SAMPLE_RATE * rev_time));
    }
    comb_out_gain = 1 - 0.366/rev_time;
}


void MoorerReverb::allocate_ringbuffers()
{
    // Allocated ringbuffers and pad them with zeros
    for(unsigned i = 0; i < get_num_input_channels(); i++)
    {
        // Tapped delay line out - one more element than the largest delay
        // Then pad out the buffer
        tapped_delay_lines.push_back(RingBuffer<SAMPLE>(tapped_delays.back()+1));
        while(tapped_delay_lines[i].get_highest_timestamp() < tapped_delays.back())
            tapped_delay_lines[i].add(0.0);

        // Comb delays - each one needs one more than the delay of that filter
        comb_delay_lines.push_back(std::vector< RingBuffer<SAMPLE> >());
        for(unsigned j = 0; j < comb_delays.size(); j++)
        {
            comb_delay_lines[i].push_back(RingBuffer<SAMPLE>(comb_delays[j]+1));
            while(comb_delay_lines[i][j].get_highest_timestamp() < comb_delays[j])
                comb_delay_lines[i][j].add(0.0);
        }

        // Outputs - each one needs one more than its delay
        tapped_delay_line_outputs.push_back(RingBuffer<SAMPLE>(tapped_out_delay+1));
        while(tapped_delay_line_outputs[i].get_highest_timestamp() < tapped_out_delay)
            tapped_delay_line_outputs[i].add(0.0);

        comb_outputs.push_back(RingBuffer<SAMPLE>(comb_out_delay+1));
        while(comb_outputs[i].get_highest_timestamp() < tapped_out_delay)
            comb_outputs[i].add(0.0);
    }
}


void MoorerReverb::filter(std::vector<SAMPLE>& input,
        std::vector<SAMPLE>& output, unsigned long t) 
{
    for(unsigned i = 0; i < input.size(); i++)
    {
        // First perform tapped delay line
        RingBuffer<SAMPLE>& tapped_delay_line = tapped_delay_lines[i];
        tapped_delay_line.add(0.0);

        SAMPLE in = input[i];
        for(unsigned j = 0; j < tapped_delays.size(); j++)
        {
            tapped_delay_line[t + tapped_delays[j]] += 
                tapped_gains[j] * in;
        }
        SAMPLE tapped_out = (in + tapped_delay_line[t])/tapped_delays.size();

        tapped_delay_line_outputs[i].add(tapped_out);

        // Then perform comb filters
        SAMPLE comb_out = 0.0;
        for(unsigned j = 0; j < comb_delays.size(); j++)
        {
            RingBuffer<SAMPLE>& buf = comb_delay_lines[i][j];
            buf.add(0.0);

            SAMPLE comb_in = tapped_out + comb_gains[j]*buf[t];
            buf[t + comb_delays[j]] += comb_in;

            comb_out += buf[t];
        }
        comb_out *= comb_out_gain/comb_delays.size();
        comb_outputs[i].add(comb_out);

        // Combine the results
        float rev_out = tapped_delay_line_outputs[i][t] + comb_outputs[i][t];
        output[i] = gain * (wetness*(rev_out) + (1.0-wetness)*input[i]);
    }
}



/*
 * ConvolutionReverb::ConvolutionReverb(unsigned impulse_length, SAMPLE* impulse,
 *                float in_gain, float in_wetness)
 *     : FilterBank(1,1), gain(in_gain), wetness(in_wetness),
 *       conv(impulse_length, impulse), wet(wetness*gain), dry((1.0-wetness)*gain),
 *       out(2)
 * {
 *     // Connect our signal chain
 *     wet.set_input_channel(conv.get_output_channel());
 *     out.set_input_channel(wet.get_output_channel(), 0);
 *     out.set_input_channel(dry.get_output_channel(), 1);
 * 
 *     output_channels.push_back(out.get_output_channel());
 * }
 * 
 * 
 * void ConvolutionReverb::set_input_channel(Channel* in_input_channel,
 *         unsigned channel_i)
 * {
 *     conv.set_input_channel(in_input_channel);
 *     dry.set_input_channel(in_input_channel);
 * }
 * 
 * 
 * void ConvolutionReverb::remove_channel(unsigned channel_i)
 * {
 *     conv.remove_channel(channel_i);
 * }
 * 
 * 
 * unsigned ConvolutionReverb::get_channel_index(Channel* channel)
 * {
 *     return conv.get_channel_index(channel);
 * }
 */




/*
 * impulse_pair* ClickTrack::impulse_from_wav(const char* filename)
 * {
 *     WavReader wav(filename);
 * 
 *     impulse_pair* out = new impulse_pair;
 *     out->num_samples = wav.get_total_samples();
 *     out->left = new SAMPLE[out->num_samples];
 *     out->right = new SAMPLE[out->num_samples];
 * 
 *     for(unsigned t = 0; t < out->num_samples; t++)
 *     {
 *         out->left[t] = wav.get_output_channel(0)->get_sample(t);
 *         out->right[t] = wav.get_output_channel(0)->get_sample(t);
 *     }
 * 
 *     return out;
 * }
 */
