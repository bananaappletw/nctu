#include <stdio.h>//基本c的函數使用
#include <stdlib.h>//基本c的函式庫使用
#include <sys/socket.h>//define一些基本的型態
#include <netinet/in.h>//socket函數使用
#include <arpa/inet.h>//define sockaddr_in和in_addr的結構
#include <string.h>//位元排序轉換整數的函數

#define PORT 25//設定port
#define BUFLEN 1024//設定buffer大小
int main()
{
	struct sockaddr_in server_addr;
	int client_sock;
	char serverbuffer[BUFLEN];
	char clientbuffer[BUFLEN];
	char response[BUFLEN];
	char response2[BUFLEN];
	client_sock=socket(AF_INET,SOCK_STREAM,0);//建立socket
	if(client_sock==-1)
	{
		printf("Cannot create socket.\n");
		return 1;
	}
	bzero(&server_addr,sizeof(server_addr));//清空ip位址
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=inet_addr("140.113.2.70");
	server_addr.sin_port=htons(PORT);
	//建立server的IPV4的socket
	if(connect(client_sock,(void*)&server_addr,sizeof(server_addr))==-1)
	{
		printf("Cannot connect to server.\n");
		return 1;
	}
	//向server要求連線要是不行顯示錯誤訊息
	bzero(serverbuffer,sizeof(serverbuffer));
	//清空buffer
	if(recv(client_sock,serverbuffer,BUFLEN,0)==-1)
	{
		printf("Cannot receive response from server.\n");
		return 1;
	}
	else
	{
		printf("%s",serverbuffer);
	}
	/*以下的型式是將使用者輸入的字串格式化再送出去*/
    //輸入hello訊息
	printf("Please enter hello message.\n");
	scanf("%s",response);
	sprintf(clientbuffer,"HELO %s\r\n",response);
	if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
	{
		printf("Cannot send request to server.\n");
		return 1;
	}
	bzero(serverbuffer,sizeof(serverbuffer));
	//清空buffer
	if(recv(client_sock,serverbuffer,BUFLEN,0)==-1)
	{
		printf("Cannot receive response from server.\n");
		return 1;
	}
	else
	{
		printf("%s",serverbuffer);
	}
	//輸入寄信者
	printf("MAIL FROM:");
	scanf("%s",response);
	sprintf(clientbuffer,"MAIL FROM:%s\r\n",response);
	if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
	{
		printf("Cannot send request to server.\n");
		return 1;
	}
	bzero(serverbuffer,sizeof(serverbuffer));
	//清空buffer
	if(recv(client_sock,serverbuffer,BUFLEN,0)==-1)
	{
		printf("Cannot receive response from server.\n");
		return 1;
	}
	else
	{
		printf("%s",serverbuffer);
	}
	printf("Please enter number of recipients\n");
	//要有幾個收信者
	int number=0;
	scanf("%d",&number);
	int i;
	for(i=0;i<number;i++)
	{
		printf("RCPT TO:");
		scanf("%s",response);
		sprintf(clientbuffer,"RCPT TO:%s\r\n",response);
		if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
		{
			printf("Cannot send request to server.\n");
			return 1;
		}
		bzero(serverbuffer,sizeof(serverbuffer));
		//清空buffer
		if(recv(client_sock,serverbuffer,BUFLEN,0)==-1)
		{
			printf("Cannot receive response from server.\n");
			return 1;
		}
		else
		{
			printf("%s",serverbuffer);
		}
	}
	//傳送DATA指令
	sprintf(clientbuffer,"DATA\r\n");
	if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
	{
		printf("Cannot send request to server.\n");
		return 1;
	}
	bzero(serverbuffer,sizeof(serverbuffer));
	//清空buffer
	if(recv(client_sock,serverbuffer,BUFLEN,0)==-1)
	{
		printf("Cannot receive response from server.\n");
		return 1;
	}
	else
	{
		printf("%s",serverbuffer);
	}
	//輸入標題
	printf("SUBJECT:");
	scanf("%s",response);
	printf("Please enter the text.\n");
	scanf("%s",response2);
	sprintf(clientbuffer,"SUBJECT:%s\r\n\r\n%s\r\n.\r\n",response,response2);
	if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
	{
		printf("Cannot send request to server.\n");
		return 1;
	}
	bzero(serverbuffer,sizeof(serverbuffer));
	//清空buffer
	if(recv(client_sock,serverbuffer,BUFLEN,0)==-1)
	{
		printf("Cannot receive response from server.\n");
		return 1;
	}
	else
	{
		printf("%s",serverbuffer);
	}
	//清除設定
	sprintf(clientbuffer,"RSET\r\n");
	if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
	{
		printf("Cannot send request to server.\n");
		return 1;
	}
	bzero(serverbuffer,sizeof(serverbuffer));
	//清空buffer
	if(recv(client_sock,serverbuffer,BUFLEN,0)==-1)
	{
		printf("Cannot receive response from server.\n");
		return 1;
	}
	else
	{
		printf("%s",serverbuffer);
	}
	//離開
	sprintf(clientbuffer,"QUIT\r\n");
	if(send(client_sock,clientbuffer,strlen(clientbuffer),0)==-1)
	{
		printf("Cannot send request to server.\n");
		return 1;
	}
	bzero(serverbuffer,sizeof(serverbuffer));
	//清空buffer
	if(recv(client_sock,serverbuffer,BUFLEN,0)==-1)
	{
		printf("Cannot receive response from server.\n");
		return 1;
	}
	else
	{
		printf("%s",serverbuffer);
	}
	close(client_sock);
	return 0;
}
