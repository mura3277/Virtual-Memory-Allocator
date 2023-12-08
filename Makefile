mymemory: shell.o mymemory.o
	gcc -o mymemory shell.o mymemory.o

shell.o: shell.c
	gcc -c shell.c

mymemory.o: mymemory.c mymemory.h
	gcc -c mymemory.c

clean: 
	rm -f mymemory
	rm -f *.o