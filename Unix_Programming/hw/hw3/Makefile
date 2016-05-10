
CC = g++
CFLAGS = -Wall 

PROGS = hw3

.PHONY: all test clean

all: $(PROGS)

hw3: main.cpp shell.o
	$(CC) -o $@ $(CFLAGS) $^
shell.o: shell.cpp
	$(CC) -c -o $@ $(CFLAGS) $^
clean:
	rm -f shell.o hw3 *.o

