
CC = g++
CFLAGS = -Wall 

PROGS = hw4

.PHONY: all test clean

all: $(PROGS)

hw4: main.cpp simple.o
	$(CC) -o $@ $(CFLAGS) $^
simple.o: simple.cpp
	$(CC) -c -o $@ $(CFLAGS) $^
clean:
	rm -f simple.o hw4 *.o

