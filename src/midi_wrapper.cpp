#include <iostream>
#include <iomanip>
#include <cmath>
#include "midi_wrapper.h"
#include "generic_instrument.h"

using namespace ClickTrack;


float ClickTrack::noteToFreq(unsigned note)
{
    return 440*pow(2, ((float)note-69)/12);
}


MidiIn::MidiIn(GenericInstrument* in_inst, int channel)
    : stream(), inst(in_inst)
{
    // If no channel specified, ask the user for a channel
    if(channel == -1)
    {
        // First list all the channels
        unsigned nPorts = stream.getPortCount();
        std::cout << std::endl << "There are " << nPorts <<
                " MIDI input sources available." << std::endl;
        std::string portName;
        for(int i=0; i<nPorts; i++)
            std::cout << "  Input Port #" << i << ": " <<
                    stream.getPortName(i) << std::endl;

        // Ask for a channel
        while(true)
        {
            std::cout << "Choose a MIDI channel: ";
            std::cin >> channel;
            if(0 <= channel && channel < nPorts)
                break;

            std::cout << "    Not a valid channel number." << std::endl;
        }
    }

    // Once we have a channel, initialize it
    stream.setCallback(&MidiIn::callback, this);
    stream.openPort(channel);
}


MidiIn::~MidiIn()
{}


void MidiIn::callback(double deltaTime, std::vector<unsigned char>* message,
                      void* in_midi)
{
    if(message->size() == 0)
    {
        std::cerr << "Ignoring empty MIDI message." << std::endl;
        return;
    }

    // Cast listener to correct type, then case on message type
    MidiIn* midi = (MidiIn*) in_midi;
    unsigned char first = message->at(0);
    unsigned char type = first >> 4;
    //unsigned char channel = first & 0x0F;
    switch(type)
    {
        case 0x9: // Note down
        {
            unsigned char note = message->at(1);
            float veloc = double(message->at(2))/100;
            midi->inst->on_note_down(note, veloc);
            break;
        }

        case 0x8: // Note up
        {
            unsigned char note = message->at(1);
            float veloc = double(message->at(2))/100;
            midi->inst->on_note_up(note, veloc);
            break;
        }

        case 0xB: // Control message 
        {
            switch(message->at(1))
            {
                case 0x01: // modulation wheel
                {
                    unsigned char value = message->at(2);
                    std::cout << "Ignoring modulation message: " << 
                        ((unsigned)value) << std::endl;
                    break;
                }

                case 0x40: // sustain pedal
                {
                    if(message->at(2) < 63)
                        midi->inst->on_sustain_up();
                    else
                        midi->inst->on_sustain_down();

                    break;
                }

                default:
                    goto UNHANDLED;
                    break;
            }
                
            break;
        }

        case 0xE: // Pitch wheel
        {
            unsigned value = (message->at(2) << 7) | message->at(1);
            midi->inst->on_pitch_wheel(value);
            break;
        }
        
        UNHANDLED:
        default:
        {
            std::cout << "Unknown messsage: 0x";
            for(int i=0; i < message->size(); i++)
                std::cout << std::hex << std::setfill('0') << std::setw(2) << 
                    (unsigned) message->at(i);
            std::cout << std::endl;

            midi->inst->on_midi_message(message);
        }
    }
}
