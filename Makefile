CC=gcc -W -Wall -O2
BUILDDIR=build

HDR_NAME=quad
SRC=\
  definitions\
  darray\
  bigint\
  hashtbl\
  hashlife\
  lifecount\
  parsers\
  runlength\
  prgrph\
  conversion
MAIN=src/main.c

OBJ=$(SRC:%=$(BUILDDIR)/%.o)
HDR=$(HDR_NAME:%=src/%.h)

hashlife: $(BUILDDIR) $(BUILDDIR)/hashlife

$(BUILDDIR):
	mkdir $(BUILDDIR)

test: hashlife
	$(BUILDDIR)/hashlife ../patterns/glider_gun.txt 0

$(BUILDDIR)/hashlife: $(HDR) $(OBJ) $(MAIN)
	$(CC) $(OBJ) $(MAIN) -o $@

$(BUILDDIR)/%.o: src/%.c
	$(CC) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

$(BUILDDIR)/%.d: src/%.c $(BUILDDIR)
	gcc -MM $< -MF $@

.PHONY: clean

.SECONDARY: $(OBJ)

include $(OBJ:.o=.d)
