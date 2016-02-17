#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <sys/time.h>
#include <vector>

using namespace std;

void bubbleSort( int[], int, int);
int quickSort( int[], int, int);
struct job{
    int method;
	int first;
	int last;
};

int a[1000000];
vector<job> work;
sem_t S, T, done;
int totalnum;
int fin=0;
int i, b, NUM;
struct timeval start1, end1;

void *func(void *arg){
    for(;;){
		sem_wait(&S);
		sem_wait(&T);
		/*
		cout<<"---------Work----------"<<endl;
		for(int p=0;p<work.size();p++)
		    cout<<work[p].method<<" "<<work[p].first<<" "<<work[p].last<<endl;
		cout<<"---------End-----------"<<endl;
		*/
		job nextjob;
		int mm, ff, ll;
		mm = work[0].method;
		ff = work[0].first;
		ll = work[0].last;
		//cout<<mm<<" "<<ff<<" "<<ll<<endl;
		work.erase(work.begin());
		sem_post(&T);
		if(mm < 3){
			int part = quickSort(a, ff, ll);
		//	cout<<"part:"<<part<<endl;
			if(mm == 0)
				nextjob.method = 1;
			else if(mm == 1)
				nextjob.method = 2;
			else 
				nextjob.method = 3;
			nextjob.first = ff;
			nextjob.last = part;
			sem_wait(&T);
			work.push_back (nextjob);
			sem_post(&T);
			sem_post(&S);
			nextjob.first = part;
			nextjob.last = ll;
			sem_wait(&T);
			work.push_back (nextjob);
			sem_post(&T);
			sem_post(&S);
		}
		else{
			bubbleSort(a, ff, ll);
			//cout<<"fin:"<<fin<<endl;
			fin++;
		}
		if(fin == 8){
		        sem_post(&done);
			break;
                }
	}
}

int main(){
	FILE *pfile, *ofile; 
	char filename[100];
	bzero(filename,100);
	pthread_t threads;
	int rc;
	int m, f, l;
	job jobs;
	cout<<"Input filename:";
	cin>>filename;
	cout<<"thread nomber:";
	cin>>NUM;
      //  printf("%s\n", filename);
	pfile = fopen(filename, "r");
	ofile = fopen("output.txt", "w");
	fscanf(pfile,"%d", &totalnum);
	for(i = 0; i < totalnum; i++)
		fscanf(pfile, "%d", &a[i]);
        sem_init(&S, 0, 0);
        sem_init(&T, 0, 1);
        sem_init(&done, 0, 0);
//	for(i=0;i<totalnum;i++)
  //      printf("%d ", a[i]);
   // printf("\n");
        jobs.method = 0;
	jobs.first = 0;
	jobs.last = totalnum-1;
	work.push_back (jobs);
	sem_post(&S);
	gettimeofday(&start1, 0);

	for(i=0;i<NUM;i++)
	    pthread_create(&threads, NULL, func, NULL);
	

	sem_wait(&done);
		for(i=0;i<totalnum;i++)
			fprintf(ofile, "%d ",a[i]);
        
		gettimeofday(&end1, 0);
		int sec = end1.tv_sec - start1.tv_sec;
		int usec = end1.tv_usec - start1.tv_usec;

		cout<<"Elased " <<sec*1000+(usec/1000.0)<<" ms"<<endl;
        /*
        int c=0;
        for(b=0;b<totalnum-1;b++){
            if(a[b] > a[b+1])
                c = 1;
        }
         
        if(c == 0)
            cout<<"Right!"<<endl;
        else
            cout<<"wrong!"<<endl;
        */
   	return 0;
    
}

void bubbleSort(int a[], int l, int r){
    int temp;
    int n,m,b; 
    //printf("l:%d\n",l);
    //printf("r:%d\n",r);
   // for(b=0;b<20;b++)
     //   printf("%d ",a[b]);
    //printf("\n");
    for(n=l;n<r+1;n++){
	    for(m=l;m<r+1+l-n;m++){
		    if(m+1<totalnum){
			    if(a[m] > a[m+1]){
			         //  printf("m:%d\n",m);
				    temp = a[m];
					a[m] = a[m+1];
					a[m+1] = temp;
				}
			}
		}
	}
}

int quickSort( int a[], int l, int r) {
    int pivot, i, j, t;
    pivot = a[l];
    i = l; 
    j = r+1;
   // printf("l:%d\n",l);
    //printf("p:%d\n",pivot);	
    while(1){
        do{ 
	        ++i;
		}while( a[i] <= pivot && i <= r );
		
   	    do{
		    --j; 
		}while( a[j] > pivot );
   	    if( i >= j ) break;
   	   //printf("%d & %d\n", i, j);
   	    t = a[i];
		a[i] = a[j];
		a[j] = t;
    }
    t = a[l];
    a[l] = a[j];
    a[j] = t;
       return j;
}
