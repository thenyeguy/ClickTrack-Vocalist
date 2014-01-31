#include <iostream>
#include <iomanip>
#include <cmath>
#include "midi_wrapper.h"
#include "generic_instrument.h"

using namespace std;
using namespace Midi;


float Midi::noteToFreq(unsigned note)
{
    return 440*pow(2, ((float)note-69)/12);
}


MidiIn::MidiIn(Instruments::GenericInstrument* in_inst, int channel)
    : stream(), inst(in_inst)
{
    // If no channel specified, ask the user for a channel
    if(channel == -1)
    {
        // First list all the channels
        unsigned nPorts = stream.getPortCount();
        cout << endl << "There are " << nPorts <<
                " MIDI input sources available." << endl;
        string portName;
        for(int i=0; i<nPorts; i++)
            cout << "  Input Port #" << i << ": " <<
                    stream.getPortName(i) << endl;

        // Ask for a channel
        while(true)
        {
            cout << "Choose a MIDI channel: ";
            cin >> channel;
            if(0 <= channel && channel < nPorts)
                break;

            cout << "    Not a valid channel number." << endl;
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
        cerr << "Ignoring empty MIDI message." << endl;
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
                    cout << "Ignoring modulation message: " << ((unsigned)value) << endl;
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
            cout << "Unknown messsage: 0x";
            for(int i=0; i < message->size(); i++)
                cout << hex << setfill('0') << setw(2) << (unsigned) message->at(i);
            cout << endl;

            midi->inst->on_midi_message(message);
        }
    }
}
