ClickTrack Vocalist
==================

This was my final project for CMU's 11-752, a speech synthesis class. The goal
was to create a synthesizer that has the sound of a human voice with the neutral
tone of a typical synthesizer, to allow for custom voice-like instruments.

Voice data is trained using MATLAB code, available in the `/mtlb/` directory.
The code takes recordings of ascending chromatic scales, one for each phoneme
being trained. It trains a set of IIR Lattice Filter coefficients and saves them
to files `/data/`.

The synthesizer then reads in this data, and is played via MIDI. Octaves 1 and 2
are used to set the attack and held voicing, while C3-E4 play voice.

For a more complete explanation of methodology, see `presenetation.pptx`.

ClickTrack
---------

This project is a fork of my audio processing toolkit, Clicktrack
([also on GitHub](https://github.com/thenyeguy/ClickTrack)).
It uses the generic audio framework provided, as well as a small number of
the pre-built filters provided. The bulk of the project is in custom code in
`/src/vocalist.h` and `/src/vocalist.cpp`. All unused code modules have been
stripped from the repo.
