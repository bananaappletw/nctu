#include<iostream>
#include<algorithm>
#include<queue>
#include<list>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cctype>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#define CLIENT_SIZE 10
using namespace std;
struct data
{
    char name[1024];
    int size;
    int state;
};
typedef struct data DATA;
struct personal_data  
{
    char name[1024];
    DATA data;
};
typedef struct personal_data PERSONAL_DATA;
struct person
{
	int sockfd;
	int filefd;
    int recvfileptr;
    int recvfilesize;
    int sendfileptr;
    int sendfilesize;
    int recvcommandptr;
    int sendcommandptr;
    int send_len;
    int recv_len;
    int read_state;
    char *sendfile_buf;
    FILE* sendfile;
    FILE* recvfile; 
    bool sending;
    bool receiving;
    char name[1024];
    char recv_buf[1024];
    char send_buf[1024];
    char recvfilename[1024];
    queue<DATA> sendqueue;
};
typedef struct person PERSON;
int main(int argc,char **argv)
{
	fd_set rset,wset;
	int maxfd;
    int listenfd,clientfd,clientfilefd,sockfd;
	int recv_len;
	int send_len;
    int flag,n;
    char username[1024];
    char filename[1024];
	char recv_buf[1024];
	char send_buf[1024];
    list<PERSONAL_DATA> ALL_DATA;
    list<PERSON> client;
    ALL_DATA.clear();
    client.clear();
    list<PERSONAL_DATA>::iterator datait;
    list<PERSON>::iterator it;
    list<PERSON>::iterator that;
	struct sockaddr_in servaddr,cliaddr;
    //PERSON temp_person;
    //DATA temp_data;
    if(argc != 2)
    {
        printf("Usage:./server.exe <port>\n");
        return 0;
    }
    memset(&servaddr,0,sizeof(servaddr));
    memset(&cliaddr,0,sizeof(cliaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(atoi(argv[1]));
    if((listenfd=socket(AF_INET,SOCK_STREAM,0)) < 0)
    {			
        printf("ERROR: Create socket error\n");
        return 0;
    }
    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) <0)
    {
        printf("ERROR: Bind error\n");
        return 0;
    }
    listen(listenfd, 100);
    /*
    flag=fcntl(listenfd,F_GETFL,0);
    fcntl(listenfd,F_SETFL,flag|O_NONBLOCK);
    */
    while(1)
    {
        //initial
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        FD_SET(listenfd,&rset);
        maxfd=listenfd;
        if(!client.empty())
        for(it=client.begin();it!=client.end();it++)
        {
            FD_SET(it->sockfd,&rset);
            if(!it->sendqueue.empty())
            {
                FD_SET(it->sockfd,&wset);
            }
            maxfd=max(maxfd,it->sockfd);
        }
        select(maxfd+1,&rset,&wset,NULL,NULL);
        if(FD_ISSET(listenfd,&rset))
        {
            //int addrlen=sizeof(cliaddr);
            //clientfd=accept(listenfd,(struct sockaddr*)&cliaddr,&addrlen);
            //sockfd
            //memset(&temp_person,0,sizeof(temp_person));
            PERSON temp_person;
            clientfd=accept(listenfd,NULL,NULL);
            temp_person.sockfd=clientfd;
            read(clientfd,recv_buf,sizeof(recv_buf));
            memset(&username,0,sizeof(username));
            if(!strncmp(recv_buf,"My_Name: ",9))
            {
                sscanf(recv_buf+9,"%s",username);
            }
            memset(&temp_person.name,0,sizeof(temp_person.name));
            memcpy(temp_person.name,username,strlen(username));
            //printf("Client: %s connect, fd = %d\n",username,clientfd);
            fflush(stdout);
            memset(&send_buf,0,sizeof(send_buf));
            snprintf(send_buf,sizeof(send_buf),"Welcome to the dropbox-like server! : %s\n",username);
            write(clientfd,send_buf,sizeof(send_buf));

            flag=fcntl(clientfd,F_GETFL,0);
            fcntl(clientfd,F_SETFL,flag|O_NONBLOCK);

            temp_person.read_state=0;
            if(!ALL_DATA.empty())
            for(datait=ALL_DATA.begin();datait!=ALL_DATA.end();datait++)
            {
                if(!strcmp(datait->name,temp_person.name))
                    temp_person.sendqueue.push(datait->data);
            }
            client.push_back(temp_person);
        }	
        if(!client.empty())
        for(it=client.begin();it!=client.end();it++)
        {
            if(FD_ISSET(it->sockfd,&rset))
            {
                if(it->read_state==0)
                {
                    /*
                    printf("read0");
                    fflush(stdout);
                    */
                    it->read_state=1;
                    it->recvcommandptr=0;
                    memset(it->recv_buf, 0, sizeof(it->recv_buf));
                    if((it->recv_len = read(it->sockfd, it->recv_buf, 1024))<0)
                    {
                        if(errno != EWOULDBLOCK)
                        {
                            printf("read error on socket\n");
                            continue;
                        }
                    }
                    else if(it->recv_len==0)
                    {
                        shutdown(sockfd,SHUT_RDWR);
                        //close(sockfd);
                        //printf("disconnect client: %s\n",it->name);
                        fflush(stdout);
                        client.erase(it);
                        break;
                    }
                    else
                    {
                        it->recvcommandptr+=it->recv_len;
                    }
                }
                else if(it->read_state==1)
                {
                    /*
                    printf("read1");
                    fflush(stdout);
                    */
                    if(it->recvcommandptr!=1024)
                    {
                        if((it->recv_len = read(it->sockfd, it->recv_buf+it->recvcommandptr, 1024-it->recvcommandptr))<0)
                        {
                            if(errno != EWOULDBLOCK)
                            {
                                printf("read error on socket\n");
                                continue;
                            }
                        }
                        else if(it->recv_len==0)
                        {
                            shutdown(sockfd,SHUT_RDWR);
                            //close(sockfd);
                            //printf("disconnect client: %s\n",it->name);
                            fflush(stdout);
                            client.erase(it);
                            break;
                        }
                        else
                        {
                            it->recvcommandptr+=it->recv_len;
                        }
                    }
                    else
                    {
                        it->read_state=2;
                        if(!strncmp(it->recv_buf,"Begin_Of_File: ",15))
                        {
                            memset(&it->recvfilename,0,sizeof(it->recvfilename));
                            sscanf(it->recv_buf+15,"%s %d",it->recvfilename,&it->recvfilesize);
                            memset(&it->recv_buf,0,sizeof(it->recv_buf));
                            it->recvfileptr=0;
                            it->recvfile=fopen(it->recvfilename,"wb");
                        }
                    }
                }
                else if(it->read_state==2)
                {
                    /*
                    printf("read2");
                    fflush(stdout);
                    */
                    if(it->recvfileptr!=it->recvfilesize)
                    {
                        n=it->recvfilesize-it->recvfileptr;
                        if(n>1024)
                            n=1024;
                        if((it->recv_len = read(it->sockfd,it->recv_buf,n))<0)
                        {
                            if(errno != EWOULDBLOCK)
                            {
                                printf("read error on socket\n");
                                continue;
                            }
                        }
                        else if(it->recv_len==0)
                        {
                            shutdown(sockfd,SHUT_RDWR);
                            //close(it->sockfd);
                            //printf("disconnect client: %s\n",it->name);
                            fflush(stdout);
                            client.erase(it);
                            break;
                        }
                        else
                        {
                            it->recvfileptr+=it->recv_len;
                            fwrite(it->recv_buf,1,it->recv_len,it->recvfile);
                            if(it->recvfileptr==it->recvfilesize)
                            {
                                /*
                                printf("succeed");
                                fflush(stdout);
                                */
                                it->read_state=0;
                                fclose(it->recvfile);
                                DATA temp_data;
                                memset(&temp_data.name,0,sizeof(temp_data.name));
                                memcpy(temp_data.name,it->recvfilename,strlen(it->recvfilename));
                                temp_data.size=it->recvfilesize;
                                temp_data.state=0;
                                for(that=client.begin();that!=client.end();that++)
                                {
                                    if(it!=that&&!strcmp(it->name,that->name))
                                        that->sendqueue.push(temp_data);
                                }
                                PERSONAL_DATA temp_personal_data;
                                memset(&temp_personal_data.name,0,sizeof(temp_personal_data));
                                memcpy(temp_personal_data.name,it->name,strlen(it->name));
                                temp_personal_data.data=temp_data;
                                ALL_DATA.push_back(temp_personal_data);
                            }
                        }
                    }
                }
            }
            if(FD_ISSET(it->sockfd,&wset))
            {					
                if(it->sendqueue.empty())
                    break;
                DATA temp_data=it->sendqueue.front();
                if(temp_data.state==0)
                {
                    /*
                    printf("state0");
                    fflush(stdout);
                    */
                    it->sendqueue.front().state=1;
                    it->sendcommandptr=0;
                    memset(it->send_buf, 0, sizeof(it->send_buf));
                    it->send_len=snprintf(it->send_buf,sizeof(it->send_buf),"Begin_Of_File: %s %d",temp_data.name,temp_data.size);
                    if((it->send_len = write(it->sockfd, it->send_buf, 1024))<0)
                    {
                        if(errno != EWOULDBLOCK)
                        {
                            printf("send error on socket\n");
                            continue;
                        }
                    }
                    else
                    {
                        it->sendcommandptr+=it->send_len;
                    }
                }
                else if(temp_data.state==1)
                {
                    /*
                    printf("state1");
                    fflush(stdout);
                    */
                    if(it->sendcommandptr!=1024)
                    {
                        if((it->send_len = write(it->sockfd, it->send_buf+it->sendcommandptr, 1024-it->sendcommandptr))<0)
                        {
                            if(errno != EWOULDBLOCK)
                            {
                                printf("send error on socket\n");
                                continue;
                            }
                        }
                        else
                        {
                            it->sendcommandptr+=it->send_len;
                        }
                    }
                    else
                    {
                        it->sendqueue.front().state=2;
                        it->sendfileptr=0;
                        it->sendfile=fopen(temp_data.name,"rb");
                        it->sendfile_buf=(char*)malloc(temp_data.size);
                        fread(it->sendfile_buf,1,temp_data.size,it->sendfile);
                    }
                }
                else if(temp_data.state==2)
                {
                    /*
                    printf("state2");
                    fflush(stdout);
                    */
                    if(it->sendfileptr!=temp_data.size)
                    {
                        if((it->send_len = write(it->sockfd, it->sendfile_buf+it->sendfileptr, temp_data.size-it->sendfileptr))<0)
                        {
                            if(errno != EWOULDBLOCK)
                            {
                                printf("send error on socket\n");
                                continue;
                            }
                        }
                        else
                        {
                            it->sendfileptr+=it->send_len;
                            if(it->sendfileptr==temp_data.size)
                            {
                                /*
                                printf("delete file %s\n",temp_data.name);
                                fflush(stdout);
                                */
                                it->sendqueue.pop();
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
