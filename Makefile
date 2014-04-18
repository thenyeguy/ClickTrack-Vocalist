# Define compiler and flags
CC      = clang++
CFLAGS  = -std=c++11 -Wall -Werror -g -I/usr/local/include
LIBS    = -L/usr/local/lib -lportaudio -lrtmidi

# Define compile paths
SRCDIR = src
MAINDIR = main

BINDIR = bin
OBJDIR = obj
vpath %.cpp $(SRCDIR):$(TSTDIR):$(MAINDIR)



# Primary target
all: vocalist 
full: clean all

# Collect all the src and object files
ALL_SRC = $(wildcard $(SRCDIR)/*.cpp)
ALL_OBJ = $(addprefix $(OBJDIR)/, $(notdir $(ALL_SRC:.cpp=.o)))


# Define main targets
vocalist: $(ALL_OBJ) $(OBJDIR)/vocalist_main.o | $(BINDIR)
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
