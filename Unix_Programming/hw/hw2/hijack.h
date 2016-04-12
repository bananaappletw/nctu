#ifndef HIJACK_H
#define HIJACK_H

#define _GNU_SOURCE
#include<stdio.h>
#include<string.h>
#include<dlfcn.h>
#include<stdarg.h>
#include<unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netdb.h>
#include<openssl/ssl.h>
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define GREEN "\033[0;32;32m"
#define YELLOW "\033[1;33m"
#define BUF_MAX 65536
char log_buf[BUF_MAX];
char char_buf[BUF_MAX];
char filelink[BUF_MAX];
char filepath[BUF_MAX];
int len;
int logfile = -1;
void log_init() __attribute__((constructor));
void log_fini() __attribute__((destructor));
void log_init();
void log_fini();
void log_it(const char* s);
void* find_func(const char* lib_name,const char* func_name);
int getaddrinfo(const char *node, const char *service,const struct addrinfo *hints,struct addrinfo **res);
FILE *fopen64(const char *filename, const char *type);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
char *getenv(const char *name);
int SSL_write(SSL *ssl, const void *buf, int num);
int SSL_read(SSL *ssl, void *buf, int num);
int fputs(const char *s, FILE *stream);
int rename(const char *old, const char *newt);
int unlink(const char *pathname);

#endif
