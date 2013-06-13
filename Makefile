#Define compiler
CC      = gcc
CFLAGS  = -std=c99 -Wall -Werror
LIBS    = -L/opt/local/lib

#Define compile paths
SRCDIR  = src
BINDIR  = bin
OBJDIR  = obj
vpath %.c $(SRCDIR)


# Primary target
all: test
full: clean all



#Define test target
TESTSRC = test.c
TESTOBJ = $(addprefix $(OBJDIR)/, $(TESTSRC:.c=.o))

test: $(TESTOBJ) $(BINDIR)
	$(CC) $(CFLAGS) $(LIBS) $(TESTOBJ) -o $(BINDIR)/$@



#Define helper macros
$(OBJDIR)/%.o: %.c $(OBJDIR)
	$(CC) -c $(CFLAGS) $< -o $@
	
$(BINDIR):
	@mkdir $(BINDIR)
	
$(OBJDIR):
	@mkdir $(OBJDIR)

clean:
	@rm -rf $(BINDIR)
	@rm -rf $(OBJDIR)
