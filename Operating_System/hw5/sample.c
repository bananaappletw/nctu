#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
int main()
{
    char inputfile[256];
	char oneline[256];
	int all_address[1048576];
	unsigned int address;
	int frame=32;
	int next;
	int i;
	int time;
	int now;
	int hit;
	int miss;
	char opcode[3];
    printf("Input filename: ");
    fflush(stdout);
    scanf("%s",inputfile);
	printf("size\tmiss\thit\tpage fault ratio\n");
	for(i=0;i<4;i++)
	{
		FILE * input=fopen(inputfile,"r");
		frame*=2;
		int* TLB=(int *)malloc(sizeof(int)*frame);
		int* frequency=(int *)malloc(sizeof(int)*frame);
		memset(TLB,-1,sizeof(TLB));
		memset(frequency,-1,sizeof(frequency));
		memset(all_address,-1,sizeof(all_address));
		hit=0;
		miss=0;
		time=0;
		next=0;
		if(!input)
		{
			printf("error input file\n");
			return 1;
		}
		while(fgets(oneline,256,input))
		{
			sscanf(oneline,"%*c%*c%*c%x",&address);
			address=address>>12;
			//printf("%d\n",address);
			if(all_address[address]<0)
			{
				//not found
				miss++;
				if(next<frame)
				{
					//not full
					TLB[next]=address;
					frequency[next]=time;
					all_address[address]=next;
					next++;				
				}
				else
				{
					//full
					//find minimum
					int j=0;
					int minimum=INT_MAX;
					int change=0;
					for(j=0;j<frame;j++)
					{
						if(frequency[j]<minimum)
						{
							minimum=frequency[j];
							change=j;
						}
					}
					all_address[TLB[change]]=-1;
					TLB[change]=address;
					frequency[change]=time;
					all_address[address]=change;		
				}
			}
			else
			{
				//found
				hit++;
				//printf("hit:%x in %d\n",address,time+1);
				//delete old frequency
				frequency[all_address[address]]=time;
			}
			time++;
		}
	printf("%d\t",frame);
    printf("%d\t",miss);
    printf("%d\t",hit);
    double miss_rate=(double)miss/(double)(hit+miss);
    printf("%f",miss_rate);	
    printf("\n");
		
	}
    return 0;
}
