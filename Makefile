OBJ=hashtbl.o hashlife.o hbitmaps.o main.o
CC=gcc -W -Wall

hashlife: $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c %.h
	$(CC) -c $<

.INTERMEDIATE: $(OBJ)

clean:
	rm -f *\~ *.o *.h.gch
