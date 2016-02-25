
GCC	= gcc
GXX	= g++
CFLAGS	= -g
LDFLAGS	=

PROGS	= inject.so getuid

all: $(PROGS)

inject.so:
	$(GCC) -o $@ -shared inject.c

%: %.c
	$(GCC) -o $@ $(CFLAGS) $(LDFLAGS) $@.c

clean:
	rm -f $(PROGS) *.o *~ a.out
