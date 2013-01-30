OBJ=hashtbl.o hashlife.o main.o

hashlife: $(OBJ)
	gcc $(OBJ) -o $@

%.o: %.c %.h
	gcc -c $<

clean:
	rm -f *\~ *.o *.h.gch
