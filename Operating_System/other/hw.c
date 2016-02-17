#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<semaphore.h>
struct thread_argument
{
	int thread_num;
};
struct thread_argument T_argument[4];
pthread_t T_thread[4];
char* inputfile[4]={
"testdata1.txt",
"testdata2.txt",
"testdata3.txt",
"testdata4.txt"
};
/*
strcpy(inputfile[0],"testdata1.txt");
inputfile[1]="testdata2.txt";
inputfile[2]="testdata3.txt";
inputfile[3]="testdata4.txt";
*/
int finish[4];
sem_t T[4];
sem_t S[4];
long long int sum[4];
void thread_task(void * argument)
{
	struct thread_argument* args=(struct thread_argument*) argument;
    FILE * input=fopen(inputfile[args->thread_num],"r");
    char buffer[256];
    long long int number;
	while(1)
	{
		sem_wait(&T[args->thread_num]);
        sum[args->thread_num]=0;
        while(fgets(buffer,256,input))
        {
            if(strncmp(buffer,"wait",4)==0)
                break;
            else
                number=atol(buffer);
            sum[args->thread_num]+=number;
        }
        if(feof(input))
            finish[args->thread_num]=1;
		sem_post(&S[args->thread_num]);
	}
}	
int main()
{
    int i;
    for(i=0;i<4;i++)
	{
		sem_init(&T[i],0,0);
		sem_init(&S[i],0,0);
        finish[i]=0;
		T_argument[i].thread_num=i;
	}
    FILE * output=fopen("Result1.txt","w");
	for(i=0;i<4;i++)
	{
		pthread_create(&T_thread[i],NULL,(void*)&thread_task,(void*)&T_argument[i]);
	}
    int value;
    int number=1;
    for(i=0;i<4;i++)
    {
        sum[i]=0;
        sem_post(&T[i]);
    }
	while(1)
	{
        for(i=0;i<4;i++)
            sem_wait(&S[i]);
        long long int all_sum=0;
        for(i=0;i<4;i++)
            all_sum+=sum[i];
        fprintf(output,"No. %d output : %lld\n",number,all_sum);
        //printf("No. %d output : %lld\n",number,all_sum);
        int allfinish=1;
        for(i=0;i<4;i++)
            if(finish[i]==0)
                allfinish=0;
        if(allfinish)
        {
            break;
        }
        for(i=0;i<4;i++)
        {
            sum[i]=0;
            sem_post(&T[i]);
        }
        number++;
	}
    return 0;
}
