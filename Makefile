CC=gcc
CFLAGS=-W -Wall -O2
#CFLAGS=-W -Wall -O0 -g
BUILDDIR=build
TESTDIR=test
SRCDIR=src

SRC=$(shell ls -1 src | grep \\.c)
OBJ=$(SRC:%.c=$(BUILDDIR)/%.o)

hashlife: $(BUILDDIR) $(BUILDDIR)/hashlife

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

$(BUILDDIR)/hashlife: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.d: $(SRCDIR)/%.c $(BUILDDIR)
	@$(CC) -MM $< -MT $(@:.d=.o) -MF $@

clean:
	rm -rf $(BUILDDIR)

.PHONY: clean

ifneq ($(MAKECMDGOALS), clean)
-include $(OBJ:.o=.d)
endif

tests: hashlife
	$(BUILDDIR)/hashlife ../patterns/glider_gun.txt 0

TEST_CHUNKS=build/chunks.o build/definitions.o test/test_chunks.c
test/test_chunks: $(TEST_CHUNKS)
	$(CC) $(CFLAGS) -Isrc $(TEST_CHUNKS) -o $@

TEST_HTBL=build/definitions.o build/chunks.o build/bigint.o build/create_quad.o build/hashtbl.o test/test_htbl.c
test/test_htbl: $(TEST_HTBL)
	$(CC) $(CFLAGS) -Isrc $(TEST_HTBL) -o $@

