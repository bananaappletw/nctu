#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

extern void random_length_task();

int main(int argc,char * argv[]) {
    if( argc != 2)
    {
        printf("Usage: ./hw1 [times]\n");
        exit(-1);
    }
    struct timeval begin;
    struct timeval end;
    int n = atoi(argv[1]);
    srand(time(0) ^ getpid());
    while(n-- > 0) {
        gettimeofday(&begin, NULL);
        long int endtime=begin.tv_sec*1000000+begin.tv_usec+100000;
        random_length_task();
        while(1)
        {
            gettimeofday(&end, NULL);
            if(end.tv_sec*1000000+end.tv_usec >= endtime)
                break;
        }
    }
    return 0;
}

