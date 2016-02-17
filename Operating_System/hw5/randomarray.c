#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<time.h>
int main()
{
    srand(time(NULL));
    int size;
    scanf("%d",&size);
    int i,j;
    for(i=0;i<size;i++)
    {
        if((i%4)<3)
            printf("I  ");
        else
            printf(" L ");
        j=rand()%INT_MAX;
        printf("%08x,1\n",j);
    }
    


    return 0;
}

