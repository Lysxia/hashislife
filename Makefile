BUILDDIR=build
TESTDIR=test
SRCDIR=src

CC=gcc -std=c99
#CFLAGS=-W -Wall -O2 -fmax-errors=2
CFLAGS=-W -Wall -O0 -g -fmax-errors=2
INCLUDES=-I$(SRCDIR)

SRC=$(shell ls -1 src | grep \\.c) \
		$(shell ls -1 src/conversion | sed "s:\(.*\.c\):conversion/\1:")
OBJ=$(SRC:%.c=$(BUILDDIR)/%.o)

hashlife: $(BUILDDIR) $(BUILDDIR)/hashlife

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BUILDDIR)/conversion

$(BUILDDIR)/hashlife: $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.d: $(SRCDIR)/%.c $(BUILDDIR)
	@$(CC) $(INCLUDES) -MM $< -MT $(@:.d=.o) -MF $@

clean:
	rm -rf $(BUILDDIR)
	rm -f $(TESTS)

.PHONY: clean

ifneq ($(MAKECMDGOALS), clean)
-include $(OBJ:.o=.d)
endif

tests: hashlife
	$(BUILDDIR)/hashlife ../patterns/glider_gun.txt 0

TESTS=test/test_chunks test/test_htbl test/test_runlength

TEST_CHUNKS=build/chunks.o build/definitions.o test/test_chunks.c
test/test_chunks: $(TEST_CHUNKS)
	$(CC) $(CFLAGS) -Isrc $(TEST_CHUNKS) -o $@

TEST_HTBL_OBJ=definitions chunks bigint create_quad hashtbl
TEST_HTBL=$(TEST_HTBL_OBJ:%=build/%.o) test/test_htbl.c
test/test_htbl: $(TEST_HTBL)
	$(CC) $(CFLAGS) -Isrc $(TEST_HTBL) -o $@

TEST_RUNLENGTH_OBJ=definitions bitmaps parsers darray runlength
TEST_RUNLENGTH=$(TEST_RUNLENGTH_OBJ:%=build/%.o) test/test_runlength.c
test/test_runlength: $(TEST_RUNLENGTH)
	$(CC) $(CFLAGS) -Isrc $(TEST_RUNLENGTH) -o $@

TEST_QTM_OBJ=definitions bigint chunks hashtbl create_quad lifecount prgrph darray conversion/quad_to_matrix
TEST_QTM=$(TEST_QTM_OBJ:%=build/%.o) test/test_quad_to_matrix.c
test/test_quad_to_matrix: $(TEST_QTM)
	$(CC) $(CFLAGS) $(INCLUDES) $(TEST_QTM) -o $@
