#Define compiler
CC      = g++
CFLAGS  = -Wall -Werror
LIBS    = -lportaudio

#Define compile paths
SRCDIR  = src
BINDIR  = bin
OBJDIR  = obj
vpath %.cpp $(SRCDIR)

# Primary target
all: test
full: clean all



#Define test target and dependencides
TESTSRC = portaudio_wrapper.cpp test.cpp
TESTOBJ = $(addprefix $(OBJDIR)/, $(TESTSRC:.cpp=.o))

test: $(TESTOBJ) $(BINDIR)
	@echo "Linking $@\n"
	@$(CC) $(CFLAGS) $(LIBS) $(TESTOBJ) -o $(BINDIR)/$@



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
