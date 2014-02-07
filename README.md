ClickTrack
==========

A simple audio processing and MIDI framework in C++.

This is a personal project of mine, in order to learn more about real time C++
and design of musical systems.  I am still fleshing out the core libraries, so
interfaces are very unstable.

Installing
----------

Currently, ClickTrack has two prerequisates: PortAudio and RtMidi. On Mac, both
are available on Homebrew. To install, run:

    brew install portaudio
    brew install rtmidi

If you are not on on Mac using homebrew, you may have to modify the library path
in the Makefile.

Once these libraries are installed, simply typing `make` will build all
executables and tests.
