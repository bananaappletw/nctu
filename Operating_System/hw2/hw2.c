#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
int main()
{
    int shm_id,children,times;
    int matrix_size,square,i,row,column,entry;
    pid_t pid;
    unsigned int* matrix,checksum;
    struct timeval start,end;
    printf("Input the matrix dimention: ");
    scanf("%d",&matrix_size);
    //initial
    square=matrix_size*matrix_size;
    if((shm_id=shmget(IPC_PRIVATE,3*matrix_size*matrix_size*sizeof(unsigned int),IPC_CREAT | 0666))<0)
    {
        perror("shmget error");
        return 1;
    }
    matrix =(unsigned int*)shmat(shm_id,NULL,0);
    //loop
    for(children=1;children<=16;children++)
    {
        //clear
        for(row=0;row<matrix_size;row++)
        {
            for(column=0;column<matrix_size;column++)
            {
                matrix[row*matrix_size+column]=row*matrix_size+column;
                matrix[row*matrix_size+column+square]=row*matrix_size+column;
            }
        }
        memset(&matrix[2*square],0,square*sizeof(unsigned int));
        /*test
        printf("A:\n");
        for(row=0;row<matrix_size;row++)
        {
            for(column=0;column<matrix_size;column++)
            {
                printf("%d ",matrix[row*matrix_size+column]);
            }
            printf("\n");
        }
        printf("B:\n");
        for(row=0;row<matrix_size;row++)
        {
            for(column=0;column<matrix_size;column++)
            {
                printf("%d ",matrix[square+row*matrix_size+column]);
            }
            printf("\n");
        }
        printf("C:\n");
        for(row=0;row<matrix_size;row++)
        {
            for(column=0;column<matrix_size;column++)
            {
                printf("%d ",matrix[2*square+row*matrix_size+column]);
            }
            printf("\n");
        }
        printf("-----------------\n");
        */
        //time start
        gettimeofday(&start,0);
        for(times=0;times<children;times++)
        {
            pid=fork();
            if(pid<0)
            {
                printf("fork error\n");
            }
            else if(pid==0)
            {
                break;
            }
            else
            {
                continue;
            }
        }
        if(pid == 0)
        {
            matrix =(unsigned int*)shmat(shm_id,NULL,0);
            for(row=(matrix_size*times)/children;row<(matrix_size*(times+1))/children;row++)
            {
                for(column=0;column<matrix_size;column++)
                {
                    for(entry=0;entry<matrix_size;entry++)
                    {
                        matrix[2*square+row*matrix_size+column]+=matrix[row*matrix_size+entry]*matrix[square+column+entry*matrix_size];
                    }
                }

            }
            shmdt(matrix);
            exit(1);
        }
        else if(pid>0)
        {
            for(i=0;i<children;i++)
                wait(NULL);
        }
        //checksum
        checksum=0;
        for(row=0;row<matrix_size;row++)
        {
            for(column=0;column<matrix_size;column++)
            {
                checksum+=matrix[2*square+row*matrix_size+column];
            }
        }
        //time end
        gettimeofday(&end, 0);
        int sec = end.tv_sec - start.tv_sec;
        int usec = end.tv_usec - start.tv_usec;
        printf("Multiplying matrices using %d process\n",children);
        printf("elapsed %f ms, Checksum: %u\n", sec*1000+(usec/1000.0),checksum);
    }
    shmdt(matrix);
    shmctl(shm_id, IPC_RMID, NULL);
    return 0;
}
