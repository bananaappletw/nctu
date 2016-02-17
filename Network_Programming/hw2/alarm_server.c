 #include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<errno.h>
#include<unistd.h>
#include <signal.h>
#include <errno.h>
#define MTU 1500
#define ALARM_TIME 10000
#define RETRY 10
void sig_alarm(int signo)
{
	return;
}
void set_alarm()
{
	signal(SIGALRM,sig_alarm);
	siginterrupt(SIGALRM,1);
}
int main(int argc,char **argv)
{
	int udpfd, clientfd,sockfd;
	struct sockaddr_in servaddr,cliaddr;
	char recv_buf[MTU];
	char send_buf[MTU];
	int n;	
	int i;
	int send_len;
	int recv_len;	
	socklen_t len;
	int ack_number;	
	int sequence_number;
	int recv_number;
	int data_len;
		
	if(argc != 3)
	{
		printf("Usage: server <port> <output_file>\n");
		return 0;
	}
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(atoi(argv[1]));
	
	if((udpfd=socket(PF_INET,SOCK_DGRAM,0)) < 0)
	{			
		printf("ERROR: Create socket error\n");
		return 0;
	}
	if(bind(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) <0)
	{
		printf("ERROR: Bind error\n");
		return 0;		
	}
	FILE* fp=fopen(argv[2],"wb");	
	if(!fp)
	{
		printf("ERROR: output file error\n");
		return 1;
	}
	set_alarm();
	sequence_number=0;
	//first receive
	for(;;)
	{
		len=sizeof(cliaddr);
		//alarm(ALARM_TIME);
		ualarm(ALARM_TIME,0);
		memset(recv_buf,0,sizeof(recv_buf));
		if((n = recvfrom(udpfd,recv_buf,MTU,0,(struct sockaddr*)&cliaddr,&len)) < 0)
		{
			if(errno=EINTR)
				continue;
		}
		else
		{
			//alarm(0);
			ualarm(0,0);
			recv_buf[n]=0;
			if(strncmp(recv_buf,"SYN",3)==0)
			{
				memset(send_buf,0,sizeof(send_buf));
				send_len=sprintf(send_buf,"ACK_OF_SYN");
				sendto(udpfd,send_buf,send_len,0,(struct sockaddr*)&cliaddr,len);	
				continue;
			}
			//printf("%s",recv_buf);
			sscanf(recv_buf,"%d%d",&recv_number,&data_len);
			break;
		}
	}
	for(;;)
	{		
		if(strncmp(recv_buf,"FIN",3)==0)
		{
			break;
		}
		else
		{
			char* pch=strchr(recv_buf,' ');
			pch++;
			pch=strchr(pch,' ');
			pch++;
			fwrite(pch,1,data_len,fp);
			if(sequence_number==recv_number)
			{
				//correct
				memset(send_buf,0,sizeof(send_buf));
				send_len=sprintf(send_buf,"ACK_OF_DATA %d",sequence_number);
				for(;;)
				{
					sendto(udpfd,send_buf,send_len,0,(struct sockaddr*)&cliaddr,len);	
					//alarm(ALARM_TIME);
					ualarm(ALARM_TIME,0);
					memset(recv_buf,0,sizeof(recv_buf));
					if((n = recvfrom(udpfd,recv_buf,MTU,0,(struct sockaddr*)&cliaddr,&len)) < 0)
					{
						if(errno=EINTR)
							continue;
					}
					else
					{
						//alarm(0);
						ualarm(0,0);
						recv_buf[n]=0;
						if(strncmp(recv_buf,"FIN",3)==0)
						{
							break;
						}
						else
						{						
							sscanf(recv_buf,"%d%d",&recv_number,&data_len);
							if(recv_number==sequence_number+1)
								break;
						}
					}
				}
			}			
		}
		sequence_number++;
	}
	memset(send_buf,0,sizeof(send_buf));
	send_len=sprintf(send_buf,"ACK_OF_FIN");
	for(i=0;i<RETRY;i++)
	{	
		if(sendto(udpfd,send_buf,send_len,0,(struct sockaddr*)&cliaddr,len))
			break;
	}
	fclose(fp);
	return 0;
}