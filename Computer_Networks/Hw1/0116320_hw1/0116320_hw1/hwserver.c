#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#define PORT 54450
#define CONNECTION_LIMIT 10
#define BUFLEN 1024
int main()
{
	struct sockaddr_in self_addr,client_addr;
	int client_addr_len,recv_len;
	int server_sock,temp_sock;
	char buffer[BUFLEN];
	server_sock=socket(AF_INET,SOCK_STREAM,0);
	if(server_sock<0)
	{
		printf("Cannot create server socket.\n");
		return 1;
	}
	bzero(&self_addr,sizeof(self_addr));
	self_addr.sin_family=AF_INET;
	self_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	self_addr.sin_port=htons(PORT);
	if(bind(server_sock,(struct sockaddr *)&self_addr,sizeof(self_addr))==-1)
	{
		printf("Cannot bind.\n");
		return 1;
	}
	if(listen(server_sock,CONNECTION_LIMIT)==-1)
	{
		printf("Cannot listen.\n");
		return 1;
	}
	while(1)
	{
       	 	temp_sock=accept(server_sock, (struct sockaddr*)&client_addr,&client_addr_len);
	        if(temp_sock==-1){
	           printf("Cannot accept request from client.\n");
	           return 1;
        }
     
        bzero(buffer,sizeof(buffer));
	recv_len=recv(temp_sock,buffer,BUFLEN,0);
	printf("Received %d bytes from client.",recv_len);
	printf("--> %s\n",buffer);
	send(temp_sock,buffer,recv_len,0);

     close(temp_sock);
     }
     return 0;
}
