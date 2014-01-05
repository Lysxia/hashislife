OBJ=darray.o bigint.o hashtbl.o hashlife.o bitmaps.o parsers.o rleparser.o plainparser.o
MAIN=main.c
CC=gcc -W -Wall

hashlife: $(OBJ) $(MAIN)
	$(CC) $(OBJ) $(MAIN) -o $@

%.o: %.c %.h
	$(CC) -c $<

.INTERMEDIATE: $(OBJ)

clean:
	rm -f *\~ *.o *.h.gch
