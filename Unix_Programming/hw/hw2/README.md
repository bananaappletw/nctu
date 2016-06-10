# Unix hw2

# There are three files in this project

- Makefile
- hijack.h
- hijack.c

# Usage

```bash
make test
```
output file is log.out

# Following functions are hooked

- getaddrinfo
- fopen64
- read
- write
- fread
- fwrite
- getenv
- SSL_write
- SSL_read
- fputs
- rename
- unlink

# Further

I using syscall to write output to file instead of using write or fwrite... library functions.

These functions will cause infinite loop when they are hooked.
