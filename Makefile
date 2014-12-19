CC=gcc -W -Wall -O0 -fmax-errors=2
BUILDDIR=build
TESTDIR=test
SRCDIR=src

SRC=$(shell ls -1 src | grep \\.c)
OBJ=$(SRC:%.c=$(BUILDDIR)/%.o)

hashlife: $(BUILDDIR) $(BUILDDIR)/hashlife

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

$(BUILDDIR)/hashlife: $(OBJ)
	$(CC) $(OBJ) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< -o $@

$(BUILDDIR)/%.d: $(SRCDIR)/%.c $(BUILDDIR)
	@gcc -MM $< -MT $(@:.d=.o) -MF $@

clean:
	rm -rf $(BUILDDIR)

.PHONY: clean

ifneq ($(MAKECMDGOALS), clean)
-include $(OBJ:.o=.d)
endif

tests: hashlife
	$(BUILDDIR)/hashlife ../patterns/glider_gun.txt 0

test/test_chunks: build/chunks.o build/definitions.o test/test_chunks.c
	$(CC) -Isrc build/chunks.o build/definitions.o test/test_chunks.c \
		-o test/test_chunks

