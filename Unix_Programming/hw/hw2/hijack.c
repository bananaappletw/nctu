#define _GNU_SOURCE
#include "hijack.h"

void log_init()
{
    logfile = open("./log.out",O_CREAT | O_RDWR | O_TRUNC,0666);
    if( logfile == -1)
    {
        logfile = fileno(stderr);
        sprintf(log_buf,RED"[ERROR] "NONE GREEN"Open logfile.out error, using stderr instead."NONE "\n");
        log_it(log_buf);
    }
    sprintf(log_buf,RED"[INFO] "NONE GREEN"Initialize logfile file"NONE "\n");
    log_it(log_buf);
}

void log_fini()
{
    sprintf(log_buf,RED"[INFO] "NONE GREEN"close file"NONE "\n");
    log_it(log_buf);
    if(logfile != fileno(stderr))
        close(logfile);
}

void log_it(const char* s)
{
    if(logfile == -1)
        log_init();
    syscall(SYS_write, logfile, s, strlen(s));
}

void* find_func(const char* lib_name,const char* func_name)
{

    void *handle = dlopen(lib_name, RTLD_LAZY);
    void* func = dlsym(handle, func_name);
    //void* func = dlsym(RTLD_NEXT, func_name);
    return func;
}

int getaddrinfo(const char *node, const char *service,const struct addrinfo *hints,struct addrinfo **res)
{
    int (*func)(const char*,const char*,const struct addrinfo *,struct addrinfo**) = find_func("libc.so.6","getaddrinfo");
    int rvalue = func(node,service,hints,res);
    sprintf(log_buf,RED"[LOG] "NONE YELLOW"getaddrinfo"NONE GREEN" connect to %s from %s"NONE "\n",node,service);
    log_it(log_buf);
    sprintf(log_buf,RED"[LOG] "NONE GREEN"Below are available addresses"NONE "\n");
    log_it(log_buf);
    struct addrinfo* ptr;
    for(ptr = *res; ptr!=NULL; ptr=ptr->ai_next)
    {
        void *addr;
        char *ipver;
        char ipstr[INET6_ADDRSTRLEN];
        if (ptr ->ai_family == AF_INET)
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)ptr->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        }
        else
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ptr->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(ptr->ai_family, addr, ipstr, sizeof ipstr);
        sprintf(log_buf,RED"[LOG] "NONE GREEN"%s: %s"NONE "\n",ipver, ipstr);
        log_it(log_buf);
    }
    return rvalue;
}

FILE *fopen64(const char *filename, const char *type)
{
    FILE *(*func)(const char*,const char*) = find_func("libc.so.6","fopen64");
    FILE * rvalue = func(filename,type);
    sprintf(log_buf,RED"[LOG] "NONE YELLOW"fopen64"NONE GREEN"(%s, mode= %s)"NONE "\n",filename,type);
    log_it(log_buf);
    return rvalue;
}

ssize_t read(int fd, void *buf, size_t count)
{
    ssize_t (*func)(int, const void*,size_t) = find_func("libc.so.6","read");
    ssize_t rvalue = func(fd,buf,count);
    if(rvalue >0)
    {
        memcpy(char_buf,buf,rvalue);
        char_buf[rvalue]= '\0';
        sprintf(log_buf,RED"[LOG] "NONE YELLOW"read"NONE GREEN" %zd bytes: %s"NONE "\n",rvalue,char_buf);
    }
    else
        sprintf(log_buf,RED"[ERROR] "NONE YELLOW"read"NONE GREEN" error happens!"NONE "\n");
    log_it(log_buf);
    return rvalue;
}

ssize_t write(int fd, const void *buf, size_t count)
{
    ssize_t (*func)(int, const void*,size_t) = find_func("libc.so.6","write");
    ssize_t rvalue = func(fd,buf,count);
    if(rvalue >0)
    {
        memcpy(char_buf,buf,rvalue);
        char_buf[rvalue]= '\0';
        sprintf(log_buf,RED"[LOG] "NONE YELLOW"write"NONE GREEN" %zd bytes: %s"NONE "\n",rvalue,char_buf);
    }
    else
        sprintf(log_buf,RED"[ERROR] "NONE YELLOW"write"NONE GREEN" error happens!"NONE "\n");
    log_it(log_buf);
    return rvalue;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t (*func)(void *, size_t, size_t, FILE*) = find_func("libc.so.6","fread");
    size_t rvalue = func(ptr,size,nmemb,stream);
    sprintf(filelink,"/proc/self/fd/%d",fileno(stream));
    if((len=readlink(filelink,filepath,BUF_MAX) !=-1))
    {
        filepath[len]= '\0';
        sprintf(log_buf,RED"[LOG] "NONE YELLOW"fread"NONE GREEN" %zu bytes from %s"NONE "\n",rvalue,filepath);
    }
    else
        sprintf(log_buf,RED"[LOG] "NONE YELLOW"fread"NONE GREEN" %zu bytes"NONE "\n",rvalue);
    log_it(log_buf);
    return rvalue;
}
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t (*func)(const void *, size_t, size_t, FILE*) = find_func("libc.so.6","fwrite");
    size_t rvalue = func(ptr,size,nmemb,stream);
    sprintf(filelink,"/proc/self/fd/%d",fileno(stream));
    if(( len=readlink(filelink,filepath,BUF_MAX)) !=-1)
    {
        filepath[len]= '\0';
        sprintf(log_buf,RED"[LOG] "NONE YELLOW"fwrite"NONE GREEN" %zu bytes to %s"NONE "\n",rvalue,filepath);
    }
    else
        sprintf(log_buf,RED"[LOG] "NONE YELLOW"fwrite"NONE GREEN" %zu bytes %s"NONE "\n",rvalue,filelink);
    log_it(log_buf);
    return rvalue;
}

char *getenv(const char *name)
{
    char *(*func)(const char*) = find_func("libc.so.6","getenv");
    char * rvalue = func(name);
    sprintf(log_buf,RED"[LOG] "NONE YELLOW"getenv" GREEN" %s = %s"NONE "\n",name,rvalue);
    log_it(log_buf);
    return rvalue;
}

int SSL_write(SSL *ssl, const void *buf, int num)
{
    int (*func)(SSL*,const void*,int) = find_func("libssl.so.1.0.0","SSL_write");
    int rvalue = func(ssl,buf,num);
    if(rvalue >0)
    {
        memcpy(char_buf,buf,rvalue);
        char_buf[rvalue]= '\0';
        sprintf(log_buf,RED"[LOG] "NONE YELLOW"SSL_write"NONE GREEN" %d bytes: %s"NONE "\n",rvalue,char_buf);
    }
    else
        sprintf(log_buf,RED"[ERROR] "NONE YELLOW"SSL_write"NONE GREEN" error happens!"NONE "\n");
    log_it(log_buf);
    return rvalue;
}

int SSL_read(SSL *ssl, void *buf, int num)
{
    int (*func)(SSL*,const void*,int) = find_func("libssl.so.1.0.0","SSL_read");
    int rvalue = func(ssl,buf,num);
    if(rvalue >0)
    {
        memcpy(char_buf,buf,rvalue);
        char_buf[rvalue]= '\0';
        sprintf(log_buf,RED"[LOG] "NONE YELLOW"SSL_read"NONE GREEN" %d bytes: %s"NONE "\n",rvalue,char_buf);
    }
    else
        sprintf(log_buf,RED"[ERROR] "NONE YELLOW"SLL_read"NONE GREEN" error happens!"NONE "\n");
    log_it(log_buf);
    return rvalue;

}

int fputs(const char *s, FILE *stream)
{
    int (*func)(const char *, FILE*) = find_func("libc.so.6","fputs");
    int rvalue = func(s,stream);
    sprintf(log_buf,RED"[LOG] "NONE YELLOW"fputs"NONE GREEN" %d bytes: %s"NONE "\n",rvalue,s);
    log_it(log_buf);
    return rvalue;
}

int rename(const char *old, const char *newt)
{
    int (*func)(const char *, const char*) = find_func("libc.so.6","rename");
    int rvalue = func(old,newt);
    sprintf(log_buf,RED"[LOG] "NONE YELLOW"rename"NONE GREEN" %s => %s"NONE "\n",old,newt);
    log_it(log_buf);
    return rvalue;
}

int unlink(const char *pathname)
{
    int (*func)(const char *) = find_func("libc.so.6","unlink");
    int rvalue = func(pathname);
    sprintf(log_buf,RED"[LOG] "NONE YELLOW"unlink"NONE GREEN"%s"NONE "\n",pathname);
    log_it(log_buf);
    return rvalue;
}
