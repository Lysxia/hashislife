#HDR=definitions.h
OBJ=definitions.o darray.o bigint.o hashtbl.o hashlife.o lifecount.o \
		parsers.o runlength.o prgrph.o conversion.o
MAIN=main.c
CC=gcc -W -Wall -O2

hashlife: $(HDR) $(OBJ) $(MAIN)
	$(CC) $(OBJ) $(MAIN) -o $@

%.o: %.c %.h
	$(CC) -c $<

.INTERMEDIATE: $(OBJ)

test: hashlife
	./hashlife ../patterns/glider_gun.txt 0

clean:
	rm -f *.o *.h.gch
