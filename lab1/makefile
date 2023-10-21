all: prog1 prog2

prog1: prog1.o
	gcc prog1.c -o prog1.o
	./prog1.o $(args)

prog2: prog2.o
	gcc prog2.c -o prog2.o
	./prog2.o $(args)

clean:
	rm -f prog1.o
	rm -f prog2.o