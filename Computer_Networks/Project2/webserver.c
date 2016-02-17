#include<stdio.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#define PORT 8000
#define CONNECTION_LIMIT 10
#define BUFLEN 1024
void urldecode(char* p);
void respond(int sockfd);
void mailclient(char* mail);
int main()
{
	struct sockaddr_in self_addr,client_addr;
	int client_addr_len,recv_len;
	int server_sock,temp_sock;
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
	if(bind(server_sock,(struct sockaddr*)&self_addr,sizeof(self_addr))==-1)
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
		client_addr_len=sizeof(client_addr);
        	temp_sock=accept(server_sock, (struct sockaddr*)&client_addr,&client_addr_len);
        	if(temp_sock<0)
		{
        	    printf("Cannot accept request from client.\n");
		    return 1;
        	}
        	else
		{
		    respond(temp_sock);
		    close(temp_sock);
     		}
	}
    return 0;
}
void respond(int sockfd)
{
    int recv_len;
    int i,j;
    char reply[BUFLEN];
    char buffer[BUFLEN];
    char method[1024];
    char directory[1024];
    bzero(buffer,sizeof(buffer));
    recv_len=read(sockfd,buffer,BUFLEN,0);
    if(recv_len>0)
    {
	//printf("This is buffer\n%s\n",buffer);
	for(i=0,j=0;i<strlen(buffer);i++)
	{
	    if(buffer[i]==' ')
    	    {
    	        method[j]='\0';
    	        i++;
    	        break;
    	    }
    	    else
    	    {
    	        method[j]=buffer[i];
    	        j++;
    	    }
	}
        i++;
   	for(j=0;i<strlen(buffer);i++)
   	{
   	    if(buffer[i]==' ')
   	    {
   	        directory[j]='\0';
   	        i++;
   	        break;
   	    }
            else
            {
            	directory[j]=buffer[i];
         	j++;
            }
	}
    	if(strncmp(directory,"mail",4)==0)
    	{
		mailclient(buffer);
		sprintf(reply,"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>succeed</body></html>");
		write(sockfd,reply,strlen(reply));
		return;
   	}
   	int ptr=open("index.html",O_RDONLY);
   	if(ptr)
   	{
		sprintf(reply,"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n");
		write(sockfd,reply,strlen(reply));
		while((recv_len=read(ptr,reply,BUFLEN))>0)
		{
			write(sockfd,reply,recv_len);
	    	}
	}
	else
	{
		sprintf(reply,"HTTP/1.1 404 Not Found");
		write(sockfd,reply,strlen(reply));
	}
   }
}
void urldecode(char* p)
{
register i=0;
while(*(p+i))
{
   if ((*p=*(p+i)) == '%')
   {
    *p=*(p+i+1) >= 'A' ? ((*(p+i+1) & 0XDF) - 'A') + 10 : (*(p+i+1) - '0');
    *p=(*p) * 16;
    *p+=*(p+i+2) >= 'A' ? ((*(p+i+2) & 0XDF) - 'A') + 10 : (*(p+i+2) - '0');
    i+=2;
   }
   else if (*(p+i)=='+')
   {
    *p=' ';
   }
   p++;
}
*p='\0';
}
void mailclient(char* mail)
{
    urldecode(mail);
    //printf("%s",mail);
    struct sockaddr_in server_addr;
    int client_sock;
    int q,w;
    char clientbuffer[BUFLEN];
    char response[BUFLEN];
    client_sock=socket(AF_INET,SOCK_STREAM,0);//«Ø¥ßsocket
    if(client_sock==-1)
    {
        printf("Cannot create socket.\n");
        return;
    }
    bzero(&server_addr,sizeof(server_addr));//²MªÅipŠì§}
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=inet_addr("140.113.2.70");
    server_addr.sin_port=htons(25);
    //«Ø¥ßserverªºIPV4ªºsocket
    if(connect(client_sock,(void*)&server_addr,sizeof(server_addr))==-1)
    {
    	printf("Cannot connect to server.\n");
    	return;
    }
    sprintf(clientbuffer,"HELO %s\r\n",response);
	if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
	{
		printf("Cannot send request to server.\n");
		return;
	}
	//²MªÅbuffer
	//¿é€J±H«HªÌ
	for(q=15,w=0;q<strlen(mail);q++)
    {
        if(mail[q]=='&'&&mail[q+1]=='t'&&mail[q+2]=='o'&&mail[q+3]=='=')
        {
            q+=4;
            response[w]='\0';
            break;
        }
        else
        {
            response[w]=mail[q];
            w++;
        }
    }
	sprintf(clientbuffer,"MAIL FROM:%s\r\n",response);
	if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
	{
		printf("Cannot send request to server.\n");
		return;
	}
	//¿é€JŠ¬«HªÌ
	for(w=0;q<strlen(mail);q++)
    {
        if(mail[q]=='&'&&mail[q+1]=='t'&&mail[q+2]=='i'&&mail[q+3]=='t'&&
           mail[q+4]=='l'&&mail[q+5]=='e'&&mail[q+6]=='=')
        {
            q+=7;
            response[w]='\0';
            break;
        }
        else
        {
            response[w]=mail[q];
            w++;
        }
    }
    sprintf(clientbuffer,"RCPT TO:%s\r\n",response);
	if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
	{
		printf("Cannot send request to server.\n");
		return;
	}
	//¶Ç°eDATA«ü¥O
	sprintf(clientbuffer,"DATA\r\n");
	if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
	{
		printf("Cannot send request to server.\n");
		return;
	}
	for(w=0;q<strlen(mail);q++)
    {
        if(mail[q]=='&'&&mail[q+1]=='c'&&mail[q+2]=='o'&&mail[q+3]=='n'&&
           mail[q+4]=='t'&&mail[q+5]=='e'&&mail[q+6]=='n'&&mail[q+7]=='t'&&mail[q+8]=='=')
        {
            q+=9;
            response[w]='\0';
            break;
        }
        else
        {
            response[w]=mail[q];
            w++;
        }
    }
	sprintf(clientbuffer,"SUBJECT:%s\r\n",response);
	if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
	{
		printf("Cannot send request to server.\n");
		return;
	}
	for(w=0;q<strlen(mail);q++)
    {
        if(mail[q]=='H'&&mail[q+1]=='T'&&mail[q+2]=='T'&&mail[q+3]=='P'&&
           mail[q+4]=='/'&&mail[q+5]=='1'&&mail[q+6]=='.'&&mail[q+7]=='1')
        {
            q+=8;
            response[w]='\0';
            break;
        }
        else
        {
            response[w]=mail[q];
            w++;
        }
    }
    sprintf(clientbuffer,"%s\r\n.\r\n",response);
    if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
	{
		printf("Cannot send request to server.\n");
		return;
	}
	sprintf(clientbuffer,"RSET\r\n");
	if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
	{
		printf("Cannot send request to server.\n");
		return;
	}
	sprintf(clientbuffer,"QUIT\r\n");
	if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
	{
		printf("Cannot send request to server.\n");
		return;
	}
	close(client_sock);
	return;
}
