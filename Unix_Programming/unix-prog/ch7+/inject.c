#include <stdio.h>
#include <sys/types.h>

uid_t getuid(void) {
	fprintf(stderr, "injected getuid funtion, always return 0\n");
	return 0;
}

