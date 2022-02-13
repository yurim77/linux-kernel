CFLAGS=-Wall 
LDFLAGS=-static

all: test1.c test2.c test3.c
	gcc -o test1 test1.c
	gcc -o test2 test2.c

.PHONY: clean

clean:
	-rm -f *~ *.o test1 test2 test3
