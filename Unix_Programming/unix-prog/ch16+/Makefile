
GCC	= gcc
GXX	= g++
CFLAGS	= -g
LDFLAGS	=

PROGS	= gethostent getprotoent getservent \
	  getaddrinfo  getnameinfo echosrv

all: $(PROGS)

%: %.c
	$(GCC) -o $@ $(CFLAGS) $(LDFLAGS) $@.c

cleanobj: clean

clean:
	rm -f $(PROGS) *.o *~ a.out
