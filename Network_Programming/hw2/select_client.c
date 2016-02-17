#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#define MTU 1500
#define MAX_DATA 1400
#define ALARM_TIME 10000
#define RETRY 10
/*
void sig_alarm(int signo)
{
	return;
}
void set_alarm()
{
	signal(SIGALRM,sig_alarm);
	siginterrupt(SIGALRM,1);
}
*/
int readable_timeo(int fd,int usec)
{
	fd_set rset;
	struct timeval tv;
	FD_ZERO(&rset);
	FD_SET(fd,&rset);
	tv.tv_sec = 0;
	tv.tv_usec = usec;
	return (select(fd+1,&rset,NULL,NULL,&tv));
}
int main(int argc, char **argv)
{
	struct sockaddr_in servaddr;
	struct hostent *host;
	int udpfd;
	int i;
	int n;
	int connect_success;
	int send_len;	
	int recv_len;
	int sequence_number;
	int ack_number;
	char recv_buf[MTU];
	char send_buf[MTU];
	char temp_buf[MAX_DATA];
	if(argc != 4)
	{
		printf("Usage: client <IP addr> <port> <input_file>\n");
		return 0;
	}
	memset(&servaddr, 0, sizeof(servaddr));
	//get the host information by the name(ip address)
	host = gethostbyname(argv[1]);
	if(host == NULL)
	{
		printf("ERROR: Get host by name error\n");
		return 0;
	}
	servaddr.sin_family = PF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	memcpy(&servaddr.sin_addr, host->h_addr, host->h_length);
	if((udpfd = socket(PF_INET, SOCK_DGRAM, 0)) <0)
	{
		printf("ERROR: Create socket error\n");
		return 0;
	}
	FILE* fp=fopen(argv[3],"rb");	
	if(!fp)
	{
		printf("ERROR: input file error\n");
		return 1;
	}
	//connect to the server specified in the servaddr
	if(connect(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) <0)
	{
		printf("ERROR: Connect to the server error\n");
		fflush(stdout);
		return 0;
	}
	//set_alarm();
	connect_success=0;
	memset(send_buf,0,sizeof(send_buf));
	send_len=sprintf(send_buf,"SYN");
	for(i=0;i<RETRY;i++)
	{
		write(udpfd,send_buf,send_len);		
		//alarm(ALARM_TIME);
		memset(recv_buf,0,sizeof(recv_buf));
		if(readable_timeo(udpfd,ALARM_TIME)==0)
		{
			continue;
			/*
			if(errno=EINTR)
			{
				continue;
			}
			*/
		}
		else
		{
			n=read(udpfd,recv_buf,MTU);
			//alarm(0);
			recv_buf[n]=0;
			if(strncmp(recv_buf,"ACK_OF_SYN",10)==0)
			{
				connect_success=1;
				break;
			}
		}
	}
	if(connect_success==0)
	{
		printf("ERROR: Connect to the server error\n");
		fflush(stdout);
		return 0;
	}
	sequence_number=0;
	for(;;)
	{
		n=fread(temp_buf,1,MAX_DATA,fp);
		if(n==0)
		{
			//reach EOF
			break;
		}
		memset(send_buf,0,sizeof(send_buf));
		send_len=sprintf(send_buf,"%d %d ",sequence_number,n);
		memcpy(send_buf+send_len,temp_buf,n);
		send_len+=n;
		for(;;)
		{
			write(udpfd,send_buf,send_len);		
			//alarm(ALARM_TIME);
			memset(recv_buf,0,sizeof(recv_buf));
			if(readable_timeo(udpfd,ALARM_TIME)==0)
			{
				continue;
				/*
				if(errno=EINTR)
				{
					continue;
				}
				*/
			}
			else
			{
				n=read(udpfd,recv_buf,MTU);
				//alarm(0);
				recv_buf[n]=0;
				if(strncmp(recv_buf,"ACK_OF_DATA ",12)==0)
				{
					sscanf(recv_buf+12,"%d",&ack_number);
					if(ack_number==sequence_number)
						break;
				}
			}
		}
		sequence_number++;
	}
	//send FIN
	memset(send_buf,0,sizeof(send_buf));
	send_len=sprintf(send_buf,"FIN");
	int error_time=0;
	for(;;)
	{
		if(error_time>RETRY)
			break;
		write(udpfd,send_buf,send_len);
		//alarm(ALARM_TIME);
		memset(recv_buf,0,sizeof(recv_buf));
		if(readable_timeo(udpfd,ALARM_TIME)==0)
		{
			error_time++;
			continue;
			/*
			if(errno=EINTR)
			{
				continue;
			}
			*/
		}
		else
		{
			n=read(udpfd,recv_buf,MTU);
			//alarm(0);
			recv_buf[n]=0;
			if(strncmp(recv_buf,"ACK_OF_FIN",10)==0)
				break;
		}
	}
	fclose(fp);
	printf("File Transfer complete\n");
	return 0;
}
