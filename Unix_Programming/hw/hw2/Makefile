
CC = gcc
CFLAGS = -Wall -shared -fPIC -ldl

PROGS = hijack.so

.PHONY: all test clean

all: $(PROGS)
hijack.so: hijack.c
	$(CC) -o $@ $(CFLAGS) $^
test: hijack.so
	LD_PRELOAD=./hijack.so wget https://www.google.com 
clean:
	rm -f *.so *html* log.out

