#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 54450
#define BUFLEN 1024

int main(int argc,char *argv[])
{
	struct sockaddr_in server_addr;
	int client_sock;
	char buffer[BUFLEN];
	char* str;

	if(argc<2)
	{
		printf("Usage : echo_client string\n");
		scanf("%s",buffer);
		str=buffer;
	}
	else
		str=argv[1];
	client_sock=socket(AF_INET,SOCK_STREAM,0);
	if(client_sock==-1){
		printf("Cannot create socket.\n");
		return 1;
	}
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=inet_addr("140.113.2.70");
	server_addr.sin_port=htons(PORT);
	if(connect(client_sock,(void*)&server_addr,sizeof(server_addr))==-1)
	{
		printf("Cannot connect to server.\n");
		return 1;
	}
	if(send(client_sock,str,strlen(str),0)==-1){
		printf("Cannot send request to server.\n");
		return 1;
	}

	bzero(buffer,sizeof(buffer));
	if(recv(client_sock,buffer,BUFLEN,0)==-1){
		printf("Cannot receive response from server.\n");
		return 1;
	}

	printf("Server says : %s\n",buffer);
	close(client_sock);
	return 0;
}
