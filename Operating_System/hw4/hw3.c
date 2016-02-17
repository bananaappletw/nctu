#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
struct thread
{
	int thread_num;
};
struct argument 
{
	int left;
	int right;
};
struct thread T_num[16];
struct argument T_args[8];
int a[1000000];
int b[1000000];
int asize;
int csize;
pthread_t T[16];
sem_t S[16];
void bubblesort(int* data,int begin,int end)
{	
	if(end<=begin)
		return;
	int i,j;
	for(i=begin;i<end;i++)
		for(j=begin;j<end+begin-i;j++)
			if(data[j]>data[j+1])
			{
				int temp;
				temp=data[j];
				data[j]=data[j+1];
				data[j+1]=temp;
			}
}
void quicksort(void* arguments)
{
	struct thread* args=arguments;
	int thread_num=args->thread_num;
	sem_wait(&S[thread_num]);
	int pivot;	
	int left=T_args[thread_num].left;
	int right=T_args[thread_num].right;
	int i=left;
	int j=right;	
	pivot=a[left];
	i++;
	if(thread_num<8)
	{		
		if(left<right)
		{
			while (1)
			{
				while (i <= right)
				{
					if (a[i] > pivot)
					{
						break;
					}
					i = i + 1;
				}
				while (j > left)
				{
					if (a[j] < pivot)
					{
						break;
					}
					j = j - 1;
				}
				if (i > j) { break; }
				int temp;
				temp=a[i];
				a[i]=a[j];
				a[j]=temp;
			}
			int temp;
			temp=a[left];
			a[left]=a[j];
			a[j]=temp;
		}
		T_args[thread_num*2].left=left;
		T_args[thread_num*2].right=j-1;
		T_args[thread_num*2+1].left=j+1;
		T_args[thread_num*2+1].right=right;	
		sem_post(&S[thread_num*2]);	
		sem_post(&S[thread_num*2+1]);
		pthread_exit(NULL);		
		return;
	}
	else
	{
		bubblesort(a,left,right);
		sem_post(&S[0]);
		pthread_exit(NULL);
		return;
	}	
}
int main()
{
    char inputfile[256];
	struct timeval start,end;
    int i,value,number;	
    asize=0;
	csize=0;
    for(i=0;i<16;i++)
	{
		sem_init(&S[i],0,0);
		T_num[i].thread_num=i;
	}
    scanf("%s",inputfile);
    FILE * input=fopen(inputfile,"r");
    FILE * output=fopen("output.txt","w");
    if(!input)
    {
        printf("error input file\n");
        return 1;
    }
    while(fscanf(input,"%d",&number))
    {
        if(feof(input))
            break;
        a[asize]=number;
        asize++;
    }
	for(i=0;i<asize;i++)
		b[i]=a[i];
	csize=asize;		
	T_args[1].left=0;	
	T_args[1].right=asize-1;
	//multithread
	gettimeofday(&start, 0);
	for(i=1;i<16;i++)
	{
		pthread_create(&T[i],NULL,(void*)&quicksort,(void*)&T_num[i]);
	}
    sem_post(&S[1]);
	while(1)
	{
		sem_getvalue(&S[0],&value);
		if(value==8)
			break;
	}
    gettimeofday(&end, 0);
    int sec = end.tv_sec - start.tv_sec;
    int usec = end.tv_usec - start.tv_usec;
	printf("multithread elapsed %f ms\n", sec*1000+(usec/1000.0));
	//singlethread
	gettimeofday(&start, 0);	
	bubblesort(b,0,csize-1);
    gettimeofday(&end, 0);
    sec = end.tv_sec - start.tv_sec;
    usec = end.tv_usec - start.tv_usec;
	printf("singlethread elapsed %f ms\n", sec*1000+(usec/1000.0));
    for(i=0;i<asize;i++)
        fprintf(output,"%d ",a[i]);		
    return 0;
}
