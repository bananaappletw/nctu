#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<map>
using namespace std;
int main()
{
    char inputfile[256];
	char oneline[256];
	int all_address[1048576];
	int all_frequency[1048576];
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
		map<int,int> frequency;
		frequency.clear();
		memset(TLB,-1,sizeof(TLB));
		memset(all_address,-1,sizeof(all_address));
		memset(all_frequency,-1,sizeof(all_frequency));
		time=0;
		next=0;
		now=0;
		hit=0;
		miss=0;
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
					all_address[address]=next;
					all_frequency[address]=time;
					frequency.insert(pair<int,int>(time,next));
					next++;				
				}
				else
				{
					//full
					//find minimum
					map<int,int>::iterator it;
					it=frequency.begin();
					int temp=it->second;
					frequency.erase(it);
					//delete old address from TLB
					all_address[TLB[temp]]=-1;
					//update the TLB			
					TLB[temp]=address;
					all_address[address]=temp;
					all_frequency[address]=time;
					//update the frequency				
					frequency.insert(pair<int,int>(time,temp));			
				}
			}
			else
			{
				//found
				hit++;
				//printf("hit:%x in %d\n",address,time+1);
				//delete old frequency
				map<int,int>::iterator it=frequency.find(all_frequency[address]);
				int temp=it->second;
				frequency.erase(it);
				//update the frequency
				all_frequency[address]=time;
				frequency.insert(pair<int,int>(time,temp));	
			}
			time++;
		}
	printf("%d\t",frame);
    printf("%d\t",miss);
    printf("%d\t",hit);
    double miss_rate=(double)miss/(double)(hit+miss);
    printf("%-1.9f",miss_rate);	
    printf("\n");
		
	}
    return 0;
}
