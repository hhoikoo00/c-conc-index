all: sequential_index concurrent_index sieve

sequential_index: Main.o Index.o
	gcc Main.o Index.o -o sequential_index -lpthread

concurrent_index: Conc.o Index.o
	gcc Conc.o Index.o -o concurrent_index -lpthread

sieve: Sieve.o Index.o
	gcc Sieve.o Index.o -o sieve -lpthread -lm

Index.o: Index.h Index.c

Main.o: Main.c Index.h

Conc.o: Conc.c Index.h

Sieve.o: Sieve.c Index.h

%.o: %.c
	gcc -c $< -lpthread

clean:
	rm -rf sequential_index concurrent_index sieve *.o

.PHONY: all clean
