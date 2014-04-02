#include <cstdlib>
#include <string>
#include "wav_reader.h"
#include "drum_machine.h"

using namespace ClickTrack;

DrumMachine::DrumMachine(const std::string& path)
    : volume(-3), adder(128), voices()
      //one for each possible MIDI channel
{
    // Connect signal chain
    volume.set_input_channel(adder.get_output_channel());

    // Set the voice
    set_voice(path);
}

void DrumMachine::set_voice(const std::string& path)
{
    // First wipe the old voice
    voices.clear();

    // Open the mapping file
    std::string mapping = path + "keymap.txt";
    std::fstream keymap;
    keymap.open(mapping);

    if(!keymap.good())
        throw InvalidKeymap(mapping);

    // Read each line
    std::string line;
    while(std::getline(keymap, line))
    {
        // Skip blank lines or comment lines
        if(line.size() == 0 || line.front() == '#')
            continue;

        // Parse out the MIDI key and file
        unsigned note = std::strtoul(line.substr(0, line.find(' ')).c_str(), 
                NULL, 0);
        std::string filename = path + line.substr(line.find(' ')+1);

        // Create a drum voice
        voices.insert(std::pair<unsigned,DrumVoice*>
                (note, new DrumVoice(filename)));
        adder.set_input_channel(voices.at(note)->get_output_channel(), note);
    }

    keymap.close();
}


DrumMachine::~DrumMachine()
{
    for(auto noteAndVoice: voices)
        delete noteAndVoice.second;
}


Channel* DrumMachine::get_output_channel()
{
    return volume.get_output_channel();
}


void DrumMachine::on_note_down(unsigned note, float velocity, 
        unsigned long time)
{
    // Ignore if this note doesn't exist
    if(voices.find(note) == voices.end())
        return;

    voices.at(note)->on_note_down(note, velocity, time);
}


// Ignore every message other than note down
void DrumMachine::on_note_up(unsigned note, float velocity, 
        unsigned long time) {}
void DrumMachine::on_sustain_down(unsigned long time) {}
void DrumMachine::on_sustain_up(unsigned long time) {}
void DrumMachine::on_pitch_wheel(unsigned value, unsigned long time) {}
void DrumMachine::on_midi_message(std::vector<unsigned char>* message, 
        unsigned long time) {}




DrumVoice::DrumVoice(const std::string& filename)
    : AudioGenerator(1), scheduler(*this), samples(), next_i(0), playing(false)
{
    // Read the wavfile into the buffer
    WavReader wav(filename.c_str());

    for(unsigned t=0; t < wav.get_total_samples(); t++)
        samples.push_back(wav.get_output_channel()->get_sample(t));
}


void DrumVoice::generate_outputs(std::vector<SAMPLE>& outputs, unsigned long t)
{
    // Run the scheduler
    scheduler.run(t);

    // Generate sample
    if(playing)
    {
        outputs[0] = samples[next_i];
        next_i++;

        if(next_i >= samples.size())
            playing = false;
    }
    else
    {
        outputs[0] = 0.0;
    }
}


void DrumVoice::on_note_down(unsigned note, float velocity, 
        unsigned long time)
{
    if(time == 0)
        time = get_next_time();

    // Put the frequency in the payload and schedule the call
    float* payload = new float;
    *payload = velocity;
    scheduler.schedule(time, DrumVoice::handle_note_down, payload);
}


void DrumVoice::handle_note_down(DrumVoice& caller, void* payload)
{
    // Get the payload
    float* velocity = (float*) payload;
    delete velocity;

    // Set state
    caller.next_i = 0;
    caller.playing = true;
}
