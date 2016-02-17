#include <iostream>
#include <algorithm>
#include <queue>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <netdb.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAXLINE 60056
using namespace std;

struct data
{
    char name[1024];
    int size;
    int state;
};
typedef struct data DATA;
int main(int argc, char **argv)
{
	fd_set rset,wset;
	int maxfd;
	struct sockaddr_in commandaddr;
	struct sockaddr_in fileaddr;
	int sockfd;
	int recv_len;
	int send_len;
    int flag,n,i;
    int filesize;
    int recvfileptr;
    int recvfilesize;
    int sendfileptr;
    int sendfilesize;
    int recvcommandptr;
    int sendcommandptr;
    int progressbar;
    int progressnow;
    char username[1024];
    char command[1024];
    char filename[1024];
    char recvfilename[1024];
	char recv_buf[1024];
	char send_buf[1024];
    char* sendfile_buf;
    FILE* sendfile;
    FILE* recvfile; 
    int read_state;
    queue<DATA> sendqueue;
    DATA temp_data;
	if(argc != 4)
	{
		printf("Usage: client <ip> <port> <username>\n");
		return 0;
	}
    memset(&username,0,sizeof(username));
    memcpy(username,argv[3],strlen(argv[3]));
	memset(&commandaddr, 0, sizeof(commandaddr));
	memset(&fileaddr, 0, sizeof(fileaddr));
    //transfer string to ip 
	if(inet_pton(AF_INET,argv[1],&commandaddr)<0)
	{
		printf("ERROR: IP transfer error\n");
		return 0;
	}
	commandaddr.sin_family = PF_INET;
	commandaddr.sin_port = htons(atoi(argv[2]));
	fileaddr.sin_family = PF_INET;
	fileaddr.sin_port = htons(atoi(argv[2]));
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0)
	{
		printf("ERROR: Create socket error\n");
		return 0;
	}
	//connect to the server specified in the commandaddr
	if(connect(sockfd, (struct sockaddr*)&commandaddr, sizeof(commandaddr)) <0)
	{
		printf("ERROR: Connect to the server error\n");
		return 0;
	}
    memset(send_buf,0,sizeof(send_buf)); 
    snprintf(send_buf,sizeof(send_buf),"My_Name: %s",username);
    write(sockfd,send_buf,sizeof(send_buf));
    read(sockfd,recv_buf,sizeof(recv_buf));
    printf("%s",recv_buf);

    flag=fcntl(STDIN_FILENO,F_GETFL,0);
    fcntl(STDIN_FILENO,F_SETFL,flag|O_NONBLOCK);
     
    flag=fcntl(sockfd,F_GETFL,0);
    fcntl(sockfd,F_SETFL,flag|O_NONBLOCK);

	maxfd=max(STDIN_FILENO,sockfd);
    read_state=0;
	while(1)
	{	
        //initial
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        FD_SET(STDIN_FILENO,&rset);
        //receive
        FD_SET(sockfd,&rset);
        //send
        if(!sendqueue.empty())
            FD_SET(sockfd,&wset);
        select(maxfd+1,&rset,&wset,NULL,NULL);
        if(FD_ISSET(STDIN_FILENO,&rset))
        {
            memset(command, 0, sizeof(command));
            fgets(command, sizeof(command), stdin);
            if(!strncmp(command,"/put ",5))
            {
                memset(&filename,0,sizeof(filename));
                sscanf(command+5,"%s",&filename);
                sendfile=fopen(filename,"rb");
                if(!sendfile)
                {
                    printf("Can't open file: %s\n",filename);
                    continue;
                }
                //add file to sendqueue
                fseek(sendfile,0,SEEK_END);
                filesize=ftell(sendfile);
                rewind(sendfile);
                memset(&temp_data,0,sizeof(temp_data));
                memcpy(temp_data.name,filename,strlen(filename));
                temp_data.size=filesize;
                temp_data.state=0;
                sendqueue.push(temp_data);
            }
            else if(!strncmp(command,"/sleep ",7))
            {
                int sleep_time;
                sscanf(command+7,"%d",&sleep_time);
                printf("Client starts to sleep\n");
                for(int i=1;i<=sleep_time;i++)
                {
                    printf("Sleep %d\n",i);
                    sleep(1);
                }
                printf("Client wakes up\n");
                continue;
            }
            else if(!strncmp(command,"/exit",5))
            {
                printf("Close connection\n");
                close(sockfd);
                break;
            }
            else
            {
                printf("Error command\n");
                continue;
            }
        }
        if(FD_ISSET(sockfd,&rset))
        {					
            if(read_state==0)
            {
                /*
                printf("read0");
                fflush(stdout);
                */
                read_state=1;
                recvcommandptr=0;
                memset(recv_buf, 0, sizeof(recv_buf));
                if((recv_len = read(sockfd, recv_buf, 1024))<0)
                {
                    if(errno != EWOULDBLOCK)
                    {
                        printf("read error on socket\n");
                        continue;
                    }
                }
                else if(recv_len==0)
                {
                    printf("Server close connection\n");
                    fflush(stdout);
                    close(sockfd);
                    return 0;
                }
                else
                {
                    recvcommandptr+=recv_len;
                }
            }
            else if(read_state==1)
            {
                /*
                printf("read1");
                fflush(stdout);
                */
                if(recvcommandptr!=1024)
                {
                    if((recv_len = read(sockfd, recv_buf+recvcommandptr, 1024-recvcommandptr))<0)
                    {
                        if(errno != EWOULDBLOCK)
                        {
                            printf("read error on socket\n");
                            continue;
                        }
                    }
                    else if(recv_len==0)
                    {
                        close(sockfd);
                        return 0;
                    }
                    else
                    {
                        recvcommandptr+=recv_len;
                    }
                }
                else
                {
                    read_state=2;
                    if(!strncmp(recv_buf,"Begin_Of_File: ",15))
                    {
                        memset(&recvfilename,0,sizeof(recvfilename));
                        sscanf(recv_buf+15,"%s%d",recvfilename,&recvfilesize);
                        progressbar=0;
                        progressnow=0;
                        recvfileptr=0;
                        recvfile=fopen(recvfilename,"wb");
                        printf("Downloading file : %s\n",recvfilename);
                        printf("Progress : [");
                    }
                }
            }
            else if(read_state==2)
            {
                /*
                printf("read2");
                fflush(stdout);
                */
                if(recvfileptr!=recvfilesize)
                {
                    n=recvfilesize-recvfileptr;
                    if(n>1024)
                        n=1024;
                    if((recv_len = read(sockfd, recv_buf,n))<0)
                    {
                        if(errno != EWOULDBLOCK)
                        {
                            printf("read error on socket\n");
                            continue;
                        }
                    }
                    else if(recv_len==0)
                    {
                        close(sockfd);
                        return 0;
                    }
                    else
                    {
                        recvfileptr+=recv_len;
                        progressnow=(recvfileptr*25)/recvfilesize;
                        if(progressnow!=progressbar)
                        {
                            for(i=0;i<progressnow-progressbar;i++)
                                printf("#");
                            progressbar=progressnow;
                        }
                        fwrite(recv_buf,1,recv_len,recvfile);
                        if(recvfileptr==recvfilesize)
                        {
                            printf("]\n");
                            printf("Download %s complete!\n",recvfilename);
                            /*
                            printf("succeed");
                            fflush(stdout);
                            */
                            read_state=0;
                            fclose(recvfile);
                        }
                    }
                }
            }
        }
        if(FD_ISSET(sockfd,&wset))
        {					
            temp_data=sendqueue.front();
            if(temp_data.state==0)
            {
                /*
                printf("state0");
                fflush(stdout);
                */
                sendqueue.front().state=1;
                sendcommandptr=0;
                memset(send_buf, 0, sizeof(send_buf));
                send_len=snprintf(send_buf,sizeof(send_buf),"Begin_Of_File: %s %d",temp_data.name,temp_data.size);
                if((send_len = write(sockfd, send_buf, 1024))<0)
                {
                    if(errno != EWOULDBLOCK)
                    {
                        printf("send error on socket\n");
                        continue;
                    }
                }
                else
                {
                    sendcommandptr+=send_len;
                }
            }
            else if(temp_data.state==1)
            {
                /*
                printf("state1");
                fflush(stdout);
                */
                if(sendcommandptr!=1024)
                {
                    if((send_len = write(sockfd, send_buf+sendcommandptr, 1024-sendcommandptr))<0)
                    {
                        if(errno != EWOULDBLOCK)
                        {
                            printf("send error on socket\n");
                            continue;
                        }
                    }
                    else
                    {
                        sendcommandptr+=send_len;
                    }
                }
                else
                {
                    sendqueue.front().state=2;
                    sendfileptr=0;
                    progressbar=0;
                    progressnow=0;
                    sendfile=fopen(temp_data.name,"rb");
                    sendfile_buf=(char*)malloc(temp_data.size);
                    fread(sendfile_buf,1,temp_data.size,sendfile);
                    printf("Uploading file : %s\n",temp_data.name);
                    printf("Progress : [");
                }
            }
            else if(temp_data.state==2)
            {
                /*
                printf("state2");
                fflush(stdout);
                */
                if(sendfileptr!=temp_data.size)
                {
                    n=temp_data.size-sendfileptr;
                    if(n>1024)
                        n=1024;
                    if((send_len = write(sockfd, sendfile_buf+sendfileptr, n))<0)
                    {
                        if(errno != EWOULDBLOCK)
                        {
                            printf("send error on socket\n");
                            continue;
                        }
                    }
                    else
                    {
                        sendfileptr+=send_len;
                        progressnow=(sendfileptr*25)/temp_data.size;
                        if(progressnow!=progressbar)
                        {
                            for(i=0;i<progressnow-progressbar;i++)
                                printf("#");
                            progressbar=progressnow;
                        }
                        if(sendfileptr==temp_data.size)
                        {
                            printf("]\n");
                            printf("Upload %s complete!\n",temp_data.name);
                            fflush(stdout);
                            sendqueue.pop();
                        }
                    }
                }
            }
        }
	}
}
