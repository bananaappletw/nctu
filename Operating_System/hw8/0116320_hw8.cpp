#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<climits>
#include<map>
using namespace std;
int main()
{
    char inputfile[256];
	char oneline[256];
	unsigned int address;
	int queue_size=64;
	int next;
	int i;
	int time;
	int head;
    int distance;
	int min_wait_time;
	int max_wait_time;
    int temp_wait_time;
	int total_wait_time;
	int total_request;
	int left_address;
	int left_time;
	int right_address;
	int right_time;
    int direction;
    printf("Input filename: ");
    fflush(stdout);
    scanf("%s",inputfile);
	printf("size\tdistance\tavg. w. t.\tmin. w. t.\tmax. w. t.\n");
	for(i=0;i<3;i++)
	{
		FILE * input=fopen(inputfile,"r");
		queue_size*=2;
		multimap<int,int> request;
        multimap<int,int>::iterator it,itlow,itup; 
		time=0;
		next=0;
		head=53;
        distance=0;
        min_wait_time=INT_MAX;
        max_wait_time=-1;
        temp_wait_time=0;
    	total_wait_time=0;
    	total_request=0;
        direction=1;
        //0 is left, 1 is right
		if(!input)
		{
			printf("error input file\n");
			return 1;
		}
		while(fgets(oneline,256,input))
		{
			sscanf(oneline,"%d",&address);
            /*
            if(next==0)
                head=address;
            */
            if(next<queue_size)
            {
                //not full
                request.insert(pair<int,int>(address,time));
                next++;
            }
            else
            {
                //queue full
                it=request.find(head); 
                if(it!=request.end())
                {
                   //not to move
                   temp_wait_time=time-(it->second);
                   total_wait_time+=temp_wait_time;
                }
                else
                {
                    //select nearest one
                    itup=request.upper_bound(head);
                    if(itup==request.begin())
                    {
                        //at the begin
                        if(direction==0)
                        {
                            distance+=2*head; 
                            time+=2*head; 
                            direction=1;
                        }
                        it=request.begin();
                        right_address=it->first;
                        right_time=it->second;
                        distance+=right_address-head; 
                        time+=right_address-head; 
                        head=right_address;
                        temp_wait_time=time-right_time;
                        total_wait_time+=temp_wait_time;
                    }
                    else if(itup==request.end())
                    {
                        //at the end
                        if(direction==1)
                        {
                            distance+=2*(255-head); 
                            time+=2*(255-head); 
                            direction=0;
                        }
                        it=request.end();
                        it--;
                        left_address=it->first;
                        left_time=it->second;
                        distance+=head-left_address; 
                        time+=head-left_address; 
                        head=left_address;
                        temp_wait_time=time-left_time;
                        total_wait_time+=temp_wait_time;
                    }
                    else
                    {
                        right_address=itup->first;
                        right_time=itup->second;
                        itlow=request.upper_bound(head);
                        itlow--;
                        left_address=itlow->first;
                        left_time=itlow->second;
                        if(direction==0)
                        //if(right_address-head > head-left_address )
                        {
                            it=itlow;
                            distance+=head-left_address; 
                            time+=head-left_address; 
                            head=left_address; 
                            temp_wait_time=time-left_time;
                            total_wait_time+=temp_wait_time;
                        }
                        else
                        {
                            it=itup;
                            distance+=right_address-head; 
                            time+=right_address-head; 
                            head=right_address; 
                            temp_wait_time=time-right_time;
                            total_wait_time+=temp_wait_time;
                        }
                    }
                }
               if(temp_wait_time<min_wait_time)
                    min_wait_time=temp_wait_time;
               if(temp_wait_time>max_wait_time)
                    max_wait_time=temp_wait_time;
                request.erase(it);
                request.insert(pair<int,int>(address,time));
            }
            total_request++;
		}
		while(!request.empty())
		{
            //queue full
            it=request.find(head); 
            if(it!=request.end())
            {
               //not to move
               temp_wait_time=time-(it->second);
               total_wait_time+=temp_wait_time;
            }
            else
            {
                //select nearest one
                itup=request.upper_bound(head);
                if(itup==request.begin())
                {
                    //at the begin
                    if(direction==0)
                    {
                        distance+=2*head; 
                        time+=2*head; 
                        direction=1;
                    }
                    it=request.begin();
                    right_address=it->first;
                    right_time=it->second;
                    distance+=right_address-head; 
                    time+=right_address-head; 
                    head=right_address;
                    temp_wait_time=time-right_time;
                    total_wait_time+=temp_wait_time;
                }
                else if(itup==request.end())
                {
                    //at the end
                    if(direction==1)
                    {
                        distance+=2*(255-head); 
                        time+=2*(255-head); 
                        direction=0;
                    }
                    it=request.end();
                    it--;
                    left_address=it->first;
                    left_time=it->second;
                    distance+=head-left_address; 
                    time+=head-left_address; 
                    head=left_address;
                    temp_wait_time=time-left_time;
                    total_wait_time+=temp_wait_time;
                }
                else
                {
                    right_address=itup->first;
                    right_time=itup->second;
                    itlow=request.upper_bound(head);
                    itlow--;
                    left_address=itlow->first;
                    left_time=itlow->second;
                    if(direction==0)
                    //if(right_address-head > head-left_address )
                    {
                        it=itlow;
                        distance+=head-left_address; 
                        time+=head-left_address; 
                        head=left_address; 
                        temp_wait_time=time-left_time;
                        total_wait_time+=temp_wait_time;
                    }
                    else
                    {
                        it=itup;
                        distance+=right_address-head; 
                        time+=right_address-head; 
                        head=right_address; 
                        temp_wait_time=time-right_time;
                        total_wait_time+=temp_wait_time;
                    }
                }
            }
           if(temp_wait_time<min_wait_time)
                min_wait_time=temp_wait_time;
           if(temp_wait_time>max_wait_time)
                max_wait_time=temp_wait_time;
            request.erase(it);
		}
		printf("%d\t",queue_size);
		printf("%d\t\t",distance);
		double avg_wait_time=(double)total_wait_time/(double)total_request;
		printf("%-1.3f\t\t",avg_wait_time);
		printf("%d\t\t",min_wait_time);	
		printf("%d",max_wait_time);	
		printf("\n");
    }
    return 0;
}
