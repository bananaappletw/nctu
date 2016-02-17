#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
struct thread_argument
{
	int thread_num;
	void (*function)(void *);
    void *function_argument;
};
struct quicksort_argument 
{
	int T_num;
	int left;
	int right;
};
struct bubblesort_argument 
{
	int* data;
	int begin;
	int end;
};
struct thread_argument T_argument[17];
struct quicksort_argument Q_argument[8]; 
struct bubblesort_argument B_argument[16];
pthread_t T_thread[17];
sem_t T[17];
sem_t S[16];
int idle[17];
int done[16];
sem_t idle_lock;
int a[1000000];
int b[1000000];
int asize;
int csize;
int totalthread;
void thread_task(void * argument)
{
	struct thread_argument* args=(struct thread_argument*) argument;
	while(1)
	{
		sem_wait(&T[args->thread_num]);
		(*(args->function))(args->function_argument);
		sem_wait(&idle_lock);
		idle[args->thread_num]=1;
		sem_post(&idle_lock);
	}
}	
int find_idle()
{	
	int i;
	for(i=1;i<=totalthread;i++)
	{
		sem_wait(&idle_lock);
		if(idle[i])
		{
			idle[i]=0;
			sem_post(&idle_lock);
			return i;
		}
		else
			sem_post(&idle_lock);
		if(i==totalthread)
			i=0;	
	}
}
void bubblesort(void* argument)
{	
	struct bubblesort_argument* args=(struct bubblesort_argument*)argument;
	int* data=args->data;
	int begin=args->begin;
	int end=args->end;
	fflush(stdout);
	if(end<=begin)
	{
		sem_post(&S[0]);	
		return;
	}
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
	sem_post(&S[0]);
	return;
}
void quicksort(void* argument)
{
	struct quicksort_argument* args=(struct quicksort_argument*)argument;
	int T_num=args->T_num;
	int pivot;	
	int left=args->left;
	int right=args->right;
	int i=left;
	int j=right;	
	pivot=a[left];
	i++;
	//printf("now is %d\n",T_num);
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
	if(T_num<=3)
	{
		Q_argument[T_num*2].left=left;
		Q_argument[T_num*2].right=j-1;
		Q_argument[T_num*2+1].left=j+1;
		Q_argument[T_num*2+1].right=right;	
	}
	else
	{
		B_argument[T_num*2].begin=left;
		B_argument[T_num*2].end=j-1;
		B_argument[T_num*2+1].begin=j+1;
		B_argument[T_num*2+1].end=right;	

	}
	sem_post(&S[T_num*2]);	
	sem_post(&S[T_num*2+1]);
	//printf("now is %d done\n",T_num);
	return;
}
int main()
{
    char inputfile[256];
    char number[256];
	struct timeval start,end;
    int i,value;
	int next_task;	
    for(i=1;i<=16;i++)
	{
		sem_init(&T[i],0,0);
		T_argument[i].thread_num=i;
	}
    for(i=0;i<=15;i++)
	{
		sem_init(&S[i],0,0);
		B_argument[i].data=a;
		done[i]=0;
	}
	for(i=1;i<=7;i++)
	{
		Q_argument[i].T_num=i;
	}
	sem_init(&idle_lock,0,0);
    printf("Input filename:");
    fflush(stdout);
    scanf("%s",inputfile);
    FILE * input=fopen(inputfile,"r");
    FILE * output=fopen("output.txt","w");
    if(!input)
    {
        printf("error input file\n");
        return 1;
    }
    fgets(number,256,input);
    asize=atoi(number);
    for(i=0;i<asize;i++)
    {
        fscanf(input,"%d",&a[i]);
		b[i]=a[i];
    }
	csize=asize;
    printf("thread number:");
    fflush(stdout);
    scanf("%d",&totalthread);
	for(i=1;i<=totalthread;i++)
	{
		pthread_create(&T_thread[i],NULL,(void*)&thread_task,(void*)&T_argument[i]);
		idle[i]=1;
	}
	Q_argument[1].left=0;
	Q_argument[1].right=asize-1;
    sem_post(&S[1]);	
	sem_post(&idle_lock);
	//multithread
	gettimeofday(&start, 0);	
	while(1)
	{
		int allready=1;
		for(i=1;i<=15;i++)
		{
			if(done[i])
				continue;
			sem_getvalue(&S[i],&value);
			if(value)
			{			
				//printf("now dispatch %d\n",i);
				next_task=find_idle();
				if(i<=7)
				{
					T_argument[next_task].function=quicksort;
					T_argument[next_task].function_argument=&Q_argument[i];
					
				}
				else
				{
					T_argument[next_task].function=bubblesort;
					T_argument[next_task].function_argument=&B_argument[i];
				}		
				sem_post(&T[next_task]);
				done[i]=1;
			}
			else
			{
				allready=0;
			}
		}
		if(allready)
			break;
	}
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
	/*gettimeofday(&start, 0);
	struct bubblesort_argument single_bubble;
	single_bubble.data=b;	
	single_bubble.begin=0;
	single_bubble.end=csize-1;
	bubblesort(&single_bubble);
    gettimeofday(&end, 0);
    sec = end.tv_sec - start.tv_sec;
    usec = end.tv_usec - start.tv_usec;
	printf("singlethread elapsed %f ms\n", sec*1000+(usec/1000.0));
    */
    for(i=0;i<asize;i++)
        fprintf(output,"%d ",a[i]);		
    return 0;
}
