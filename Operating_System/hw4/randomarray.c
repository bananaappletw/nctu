#include<stdio.h>
#include<stdlib.h>
int main()
{
    int size;
    scanf("%d",&size);
    int i,j;
    printf("%d \n",size);
    for(i=0;i<size;i++)
    {
        j=rand()%100000;
        printf("%d ",j);
    }
    return 0;
}

