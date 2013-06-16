#Define compiler
CC      = clang++
CFLAGS  = -Wall -Werror
LIBS    = -lportaudio

#Define compile paths
SRCDIR  = src
BINDIR  = bin
OBJDIR  = obj
vpath %.cpp $(SRCDIR)

# Primary target
all: tests
full: clean all

tests: test_ringbuffer test_portaudio test_filterchain



#Define test target and dependencides
TEST_RINGBUFFER_SRC = test_ringbuffer.cpp
TEST_RINGBUFFER_OBJ = $(addprefix $(OBJDIR)/, $(TEST_RINGBUFFER_SRC:.cpp=.o))
test_ringbuffer: $(TEST_RINGBUFFER_OBJ) $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $(TEST_RINGBUFFER_OBJ) -o $(BINDIR)/$@


TEST_PORTAUDIO_SRC = portaudio_wrapper.cpp test_portaudio.cpp
TEST_PORTAUDIO_OBJ = $(addprefix $(OBJDIR)/, $(TEST_PORTAUDIO_SRC:.cpp=.o))
test_portaudio: $(TEST_PORTAUDIO_OBJ) $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $(TEST_PORTAUDIO_OBJ) -o $(BINDIR)/$@


TEST_FILTERCHAIN_SRC = portaudio_wrapper.cpp filterblock.cpp ioblocks.cpp \
                       test_portaudio.cpp
TEST_FILTERCHAIN_OBJ = $(addprefix $(OBJDIR)/, $(TEST_FILTERCHAIN_SRC:.cpp=.o))
test_filterchain: $(TEST_FILTERCHAIN_OBJ) $(BINDIR)
	@echo "Linking $(BINDIR)/$@...\n"
	@$(CC) $(CFLAGS) $(LIBS) $(TEST_FILTERCHAIN_OBJ) -o $(BINDIR)/$@


#Define helper macros
$(OBJDIR)/%.o: %.cpp $(OBJDIR)
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
