BUILDDIR=build
TESTDIR=test
SRCDIR=src

CC=gcc -std=c99
#CFLAGS=-W -Wall -O2 -fmax-errors=2
CFLAGS=-W -Wall -O0 -g -fmax-errors=2
INCLUDES=-Iinclude

SRC=$(shell ls -1 src | grep \\.c) \
		$(shell ls -1 src/conversion | sed "s:\(.*\.c\):conversion/\1:")
OBJ=$(SRC:%.c=$(BUILDDIR)/%.o)
MAIN=main/main.c

hashlife: $(BUILDDIR) $(BUILDDIR)/hashlife

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BUILDDIR)/conversion

$(BUILDDIR)/hashlife: $(OBJ) $(BUILDDIR)/main.o
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) $(BUILDDIR)/main.o -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/main.o: $(MAIN)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.d: $(SRCDIR)/%.c $(BUILDDIR)
	@$(CC) $(INCLUDES) -MM $< -MT $(@:.d=.o) -MF $@

$(BUILDDIR)/main.d: $(MAIN) $(BUILDDIR)
	@$(CC) $(INCLUDES) -MM $< -MT $(@:.d=.o) -MF $@

clean:
	rm -rf $(BUILDDIR)
	rm -f $(TESTS)

.PHONY: clean

ifneq ($(MAKECMDGOALS), clean)
-include $(OBJ:.o=.d) $(BUILDDIR)/main.d
endif

tests: hashlife
	$(BUILDDIR)/hashlife ../patterns/glider_gun.txt 0

TESTS=chunks htbl runlength quadtomatrix rletoquad

test/test_%: test/test_%.c $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) $< -o $@
