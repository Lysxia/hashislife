BUILDDIR=build
SRCDIR=main src test

CC=gcc -std=c99
#CFLAGS=-W -Wall -O2 -fmax-errors=2
CFLAGS=-W -Wall -O0 -g -fmax-errors=2
INCLUDES=-Iinclude

OBJ=$(shell find src -type f -name \*.c | sed 's:^\(.*\)\.c:$(BUILDDIR)/\1\.o:')
ALLSRC=$(shell find $(SRCDIR) -type f -name \*.c) $(shell find lex -type f -name \*.l | sed 's/\.l/\.c/')
DEPENDS=$(ALLSRC:%.c=$(BUILDDIR)/%.d)
#BUILDSUBDIR=$(shell find $(SRCDIR) -type d | sed s:^:$(BUILDDIR)/:)
MAIN=main/main

hashlife: $(BUILDDIR)/hashlife

$(BUILDDIR)/hashlife: $(BUILDDIR)/$(MAIN)
	ln $< $@

$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/$(MAIN).o: $(MAIN).c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.d: %.c
	mkdir -p $(@D)
	$(CC) $(INCLUDES) -MM $< -MT $(@:.d=.o) -MF $@

$(BUILDDIR)/%: $(BUILDDIR)/%.o $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) $< -o $@

clean:
	rm -rf $(BUILDDIR)
	rm -f lex/*.c

.PHONY: builddir clean

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPENDS)
endif

