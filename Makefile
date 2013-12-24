#Define compiler and flags
CC      = clang++
CFLAGS  = -I/usr/local/include -Wall -Wno-delete-non-virtual-dtor -Werror -g 
LIBS    = -L/usr/local/lib -lportaudio -lrtmidi

#Define compile paths
SRCDIR  = src
TSTDIR  = tst
BINDIR  = bin
OBJDIR  = obj
vpath %.cpp $(SRCDIR):$(TSTDIR)

# Primary target
all: tests
full: clean all

tests: test_ringbuffer test_fft test_filterchain test_wav test_convolve \
       test_reverb test_midi test_subtractive_synth


# Macro the filter list
IOCORE_SRC = portaudio_wrapper.cpp fft.cpp filter_generics.cpp \
             io_elements.cpp oscillator.cpp
FILTER_SRC = elementary_filters.cpp convolve.cpp delay.cpp reverb.cpp

INSTRUMENTCORE_SRC = midi_wrapper.cpp generic_instrument.cpp
INSTRUMENT_SRC = simple_midi_instrument.cpp subtractive_synth.cpp

ALL_SRC = $(IOCORE_SRC) $(FILTER_SRC) $(INSTRUMENTCORE_SRC) $(INSTRUMENT_SRC)


#Define test target and dependencides
TEST_RINGBUFFER_SRC = test_ringbuffer.cpp
TEST_RINGBUFFER_OBJ = $(addprefix $(OBJDIR)/, $(TEST_RINGBUFFER_SRC:.cpp=.o))
test_ringbuffer: $(TEST_RINGBUFFER_OBJ) | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $(TEST_RINGBUFFER_OBJ) -o $(BINDIR)/$@


TEST_FFT_SRC = fft.cpp test_fft.cpp
TEST_FFT_OBJ = $(addprefix $(OBJDIR)/, $(TEST_FFT_SRC:.cpp=.o))
test_fft: $(TEST_FFT_OBJ) | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $(TEST_FFT_OBJ) -o $(BINDIR)/$@


TEST_FILTERCHAIN_SRC = $(IOCORE_SRC) $(FILTER_SRC) test_filterchain.cpp
TEST_FILTERCHAIN_OBJ = $(addprefix $(OBJDIR)/, $(TEST_FILTERCHAIN_SRC:.cpp=.o))
test_filterchain: $(TEST_FILTERCHAIN_OBJ) | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $(TEST_FILTERCHAIN_OBJ) -o $(BINDIR)/$@


TEST_WAV_SRC = $(IOCORE_SRC) test_wav.cpp
TEST_WAV_OBJ = $(addprefix $(OBJDIR)/, $(TEST_WAV_SRC:.cpp=.o))
test_wav: $(TEST_WAV_OBJ) | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $(TEST_WAV_OBJ) -o $(BINDIR)/$@


TEST_CONVOLVE_SRC = $(IOCORE_SRC) $(FILTER_SRC) test_convolve.cpp
TEST_CONVOLVE_OBJ = $(addprefix $(OBJDIR)/, $(TEST_CONVOLVE_SRC:.cpp=.o))
test_convolve: $(TEST_CONVOLVE_OBJ) | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $(TEST_CONVOLVE_OBJ) -o $(BINDIR)/$@


TEST_REVERB_SRC = $(IOCORE_SRC) $(FILTER_SRC) test_reverb.cpp
TEST_REVERB_OBJ = $(addprefix $(OBJDIR)/, $(TEST_REVERB_SRC:.cpp=.o))
test_reverb: $(TEST_REVERB_OBJ) | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $(TEST_REVERB_OBJ) -o $(BINDIR)/$@


TEST_MIDI_SRC = $(ALL_SRC) test_midi.cpp
TEST_MIDI_OBJ = $(addprefix $(OBJDIR)/, $(TEST_MIDI_SRC:.cpp=.o))
test_midi: $(TEST_MIDI_OBJ) | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $(TEST_MIDI_OBJ) -o $(BINDIR)/$@


TEST_SUBTRACTIVE_SYNTH_SRC = $(ALL_SRC) test_subtractive_synth.cpp
TEST_SUBTRACTIVE_SYNTH_OBJ = $(addprefix $(OBJDIR)/, $(TEST_SUBTRACTIVE_SYNTH_SRC:.cpp=.o))
test_subtractive_synth: $(TEST_SUBTRACTIVE_SYNTH_OBJ) | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $(TEST_SUBTRACTIVE_SYNTH_OBJ) -o $(BINDIR)/$@


#Define helper macros
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	@echo "Compiling $<"
	@$(CC) -c $(CFLAGS) $< -o $@
	
$(BINDIR):
	@mkdir $(BINDIR)
	
$(OBJDIR):
	@mkdir $(OBJDIR)

clean:
	@echo "Cleaning...\n"
	@rm -rf $(BINDIR)
	@rm -rf $(OBJDIR)
