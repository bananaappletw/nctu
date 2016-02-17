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
int main(int argc, char **argv)
{
	
	fd_set rset,allset;
	int maxfd;
	struct sockaddr_in servaddr;
	struct hostent *host;
	int fd,nready;
	int recv_len;
	char recv_buf[1024];
	char send_buf[1024];
	if(argc != 3)
	{
		printf("Usage: client <IP addr> <port>\n");
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
	if((fd = socket(AF_INET, SOCK_STREAM, 0)) <0)
	{
		printf("ERROR: Create socket error\n");
		return 0;
	}
	//connect to the server specified in the servaddr
	if(connect(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) <0)
	{
		printf("ERROR: Connect to the server error\n");
		return 0;
	}
	FD_ZERO(&allset);
	FD_SET(STDIN_FILENO,&allset);
	FD_SET(fd,&allset);
	maxfd=STDIN_FILENO;
	if(fd>maxfd)
	maxfd=fd;
	while(1)
	{	
		rset=allset;
		nready=select(maxfd+1,&rset,NULL,NULL,NULL);
		if(FD_ISSET(STDIN_FILENO,&rset))
		{
			memset(send_buf, 0, sizeof(send_buf));
			fgets(send_buf, sizeof(send_buf), stdin);
			write(fd, send_buf, strlen(send_buf));
			if(--nready<=0)
				continue;
		}
		else if(FD_ISSET(fd,&rset))
		{					
			memset(recv_buf, 0, sizeof(recv_buf));
			recv_len = read(fd, recv_buf, sizeof(recv_buf));
			if(recv_len == 0)
			{
				printf("Server close the connection\n");
				return 0;
			}
			else if(recv_len <0)
			{
				printf("ERROR: Read from the server error\n");
			}
			else
			{
				printf("%s", recv_buf);
			}
			if(--nready<=0)
				continue;
		}
	}
}
