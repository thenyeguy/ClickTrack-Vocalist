# Define compiler and flags
CC      = clang++
CFLAGS  = -std=c++11 -Wall -Werror -g -I/usr/local/include
LIBS    = -L/usr/local/lib -lportaudio -lrtmidi -lfftw3 -lm

# Define compile paths
SRCDIR = src
TSTDIR = test
MAINDIR = main

BINDIR = bin
OBJDIR = obj
vpath %.cpp $(SRCDIR):$(TSTDIR):$(MAINDIR)



# Primary target
all: targets tests
full: clean all

# List of output targets
targets: subtractive_synth drum_machine
tests: test_ringbuffer test_fft test_filterchain test_wav test_convolve \
       test_reverb test_filters test_oscillators

# Collect all the src and object files
ALL_SRC = $(wildcard $(SRCDIR)/*.cpp)
ALL_OBJ = $(addprefix $(OBJDIR)/, $(notdir $(ALL_SRC:.cpp=.o)))


# Define main targets
subtractive_synth: $(ALL_OBJ) $(OBJDIR)/subtractive_synth_main.o | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $^ -o $(BINDIR)/$@

drum_machine: $(ALL_OBJ) $(OBJDIR)/drum_machine_main.o | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $^ -o $(BINDIR)/$@


# Define test targets
test_ringbuffer: $(ALL_OBJ) $(OBJDIR)/test_ringbuffer.o | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $^ -o $(BINDIR)/$@

test_fft: $(ALL_OBJ) $(OBJDIR)/test_fft.o | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $^ -o $(BINDIR)/$@

test_filterchain: $(ALL_OBJ) $(OBJDIR)/test_filterchain.o | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $^ -o $(BINDIR)/$@

test_wav: $(ALL_OBJ) $(OBJDIR)/test_wav.o | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $^ -o $(BINDIR)/$@

test_convolve: $(ALL_OBJ) $(OBJDIR)/test_convolve.o | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $^ -o $(BINDIR)/$@

test_reverb: $(ALL_OBJ) $(OBJDIR)/test_reverb.o | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $^ -o $(BINDIR)/$@

test_filters: $(ALL_OBJ) $(OBJDIR)/test_filters.o | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $^ -o $(BINDIR)/$@

test_oscillators: $(ALL_OBJ) $(OBJDIR)/test_oscillators.o | $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $^ -o $(BINDIR)/$@



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
