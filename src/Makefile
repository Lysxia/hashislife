HDR=quad.h
SRC=definitions\
    darray\
    bigint\
    hashtbl\
    hashlife\
    lifecount\
		parsers\
    runlength\
    prgrph\
    conversion
MAIN=main.c
CC=gcc -W -Wall -O2
OBJ=$(SRC:%=%.o)

hashlife: $(HDR) $(SRC:%=%.o) $(MAIN)
	$(CC) $(OBJ) $(MAIN) -o $@

%.o: %.c %.h
	$(CC) -c $<

.INTERMEDIATE: $(OBJ)

test: hashlife
	./hashlife ../patterns/glider_gun.txt 0

clean:
	rm -f *.o *.h.gch
