OBJ=read_gol.o sparsemap.o slowlife.o

slowlife:

test: slowlife test.txt
	./slowlife test.txt

slowlife: $(OBJ)
	gcc $(OBJ) -o slowlife

%.o: %.c %.h
	gcc -c $<

clean:
	rm -f *\~ *.o *.h.gch
