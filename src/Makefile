#HDR=definitions.h
OBJ=definitions.o darray.o bigint.o hashtbl.o hashlife.o lifecount.o \
		parsers.o runlength.o prgrph.o conversion.o
MAIN=main.c
CC=gcc -W -Wall

hashlife: $(HDR) $(OBJ) $(MAIN)
	$(CC) $(OBJ) $(MAIN) -o $@

%.o: %.c %.h
	$(CC) -c $<

.INTERMEDIATE: $(OBJ)

clean:
	rm -f *.o *.h.gch
