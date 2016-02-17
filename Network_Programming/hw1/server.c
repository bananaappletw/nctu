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
#define CLIENT_SIZE 10
struct person
{
	int fd;
	char name[256];
	char ip[256];
	int port;
};
int main(int argc,char **argv)
{
		int listenfd, clientfd,sockfd;
		int recv_len;
		char buffer[1024],command[256],pastname[256],newname[256],message[256],tellperson[256];
		int i,j,k,n,maxfd,maxi;
		int nready;
		struct person client [CLIENT_SIZE];
		fd_set rset,allset;
		struct sockaddr_in servaddr,cliaddr;
			
		if(argc != 2)
		{
			printf("Usage: server <port>\n");
			return 0;
		}
		listenfd = socket(AF_INET,SOCK_STREAM,0);
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
		listen(listenfd, 10);
		maxfd=listenfd;
		maxi=-1;
		for(i=0;i<CLIENT_SIZE;i++)
			client[i].fd=-1;
		FD_ZERO(&allset);
		FD_SET(listenfd,&allset);
		while(1)
		{
			rset = allset;
			nready = select(maxfd+1,&rset,NULL,NULL,NULL);
			if(FD_ISSET(listenfd,&rset))
			{
				int addrlen=sizeof(cliaddr);
				clientfd=accept(listenfd,(struct sockaddr*)&cliaddr,&addrlen);
				for(i=0;i<CLIENT_SIZE;i++)
				{
					if(client[i].fd<0)
					{
						client[i].fd=clientfd;
						strcpy(client[i].name,"anonymous");
						break;
					}
				}
				if(i==CLIENT_SIZE)
					perror("too many clients");	
				strcpy(client[i].ip,inet_ntop(AF_INET, &cliaddr.sin_addr,client[i].ip,sizeof(client[i])));
				client[i].port=ntohs(cliaddr.sin_port);
				
				snprintf(buffer,sizeof(buffer),"[Server] Hello, anonymous! From: %s/%d\n",client[i].ip,client[i].port);
				write(client[i].fd,buffer,strlen(buffer));				
				FD_SET(client[i].fd,&allset);
				
				if(clientfd>maxfd)
					maxfd=clientfd;
				if(i>maxi)
					maxi=i;
				for(j=0;j<=maxi;j++)
				{
					if(FD_ISSET(client[j].fd,&allset))
					{
						if(client[j].fd!=client[i].fd)
						{				
							memset(buffer,0,sizeof(buffer));
							snprintf(buffer,sizeof(buffer),"[Server] Someone is coming!\n");
							write(client[j].fd,buffer,strlen(buffer));
						}
					}
				}
				if (--nready <= 0)
					continue;
			}	
			for(i=0;i <= maxi;i++)
			{
				if((sockfd = client[i].fd) < 0)
					continue;
				if(FD_ISSET(sockfd,&rset))
				{
					
					memset(buffer,0,sizeof(buffer));
					while((n=read(sockfd,buffer,sizeof(buffer)))<0)
						if(errno==EINTR)
							continue;
					if(n==0)
					{
						//someone logoff
						for(j=0;j<=maxi;j++)
						{
							if(FD_ISSET(client[j].fd,&allset))
							{
								if(client[j].fd!=client[i].fd)
								{				
									memset(buffer,0,sizeof(buffer));
									snprintf(buffer,sizeof(buffer),"[Server] %s is offline.\n",client[i].name);
									write(client[j].fd,buffer,strlen(buffer));
								}
							}
						}						
						close(sockfd);
						FD_CLR(sockfd,&allset);
						client[i].fd=-1;					
					}
					else
					{			
						memset(command,0,sizeof(command));
						sscanf(buffer,"%s",command);
						if(strcmp(command,"who")==0)
						{
							for(j=0;j<=maxi;j++)
							{
								if(FD_ISSET(client[j].fd,&allset))
								{
									memset(buffer,0,sizeof(buffer));
									if(client[j].fd!=client[i].fd)
										snprintf(buffer,sizeof(buffer),"[Server] %s %s/%d\n",client[j].name,client[j].ip,client[j].port);
									else									
										snprintf(buffer,sizeof(buffer),"[Server] %s %s/%d <-me\n",client[j].name,client[j].ip,client[j].port);
									write(client[i].fd,buffer,strlen(buffer));
								}
							}												
						}
						else if(strcmp(command,"name")==0)
						{
							char* pch;
							pch = strchr(buffer,' ');
							if(pch!=NULL)
							{							
								pch++;
								strcpy(newname,pch);
								newname[strlen(newname)-1]='\0';
								int same=0;
								for(j=0;j<=maxi;j++)
								{
									if(FD_ISSET(client[j].fd,&allset)&&strcmp(client[j].name,newname)==0)
										same=1;
								}				
								int inavailable=0;
								int x;
								for(x=0;x<strlen(newname);x++)
								{
									if(!isalpha(newname[x]))
										inavailable=1;								
								}
								if(strlen(newname)<2||strlen(newname)>12)
									inavailable=1;
								if(strcmp(newname,"anonymous")==0)
								{								
									memset(buffer,0,sizeof(buffer));								
									snprintf(buffer,sizeof(buffer),"[Server] ERROR: Username cannot be anonymous.\n");
									write(client[i].fd,buffer,strlen(buffer));
								}
								else if(same)
								{
									memset(buffer,0,sizeof(buffer));								
									snprintf(buffer,sizeof(buffer),"[Server] ERROR: <NEW USERNAME> has been used by others.\n");
									write(client[i].fd,buffer,strlen(buffer));							
								}
								else if(inavailable)
								{
									memset(buffer,0,sizeof(buffer));								
									snprintf(buffer,sizeof(buffer),"[Server] ERROR: Username can only consists of 2~12 English letters.\n");
									write(client[i].fd,buffer,strlen(buffer));									
								}
								else
								{
									strcpy(pastname,client[i].name);
									strcpy(client[i].name,newname);
									memset(buffer,0,sizeof(buffer));									
									snprintf(buffer,sizeof(buffer),"[Server] You're known as %s.\n",client[i].name);
									write(client[i].fd,buffer,strlen(buffer));
									for(j=0;j<=maxi;j++)
									{
										if(FD_ISSET(client[j].fd,&allset))
										{
											if(client[j].fd!=client[i].fd)
											{				
												memset(buffer,0,sizeof(buffer));
												snprintf(buffer,sizeof(buffer),"[Server] %s is known as %s.\n",pastname,newname);
												write(client[j].fd,buffer,strlen(buffer));
											}
										}
									}							
								}
							}
							else
							{
								memset(buffer,0,sizeof(buffer));
								snprintf(buffer,sizeof(buffer),"[Server] ERROR: Error command.\n",client[i].name,message);						
								write(sockfd,buffer,strlen(buffer));
							}
						}
						else if(strcmp(command,"yell")==0)
						{							
							char* pch;
							pch = strchr(buffer,' ');
							if(pch!=NULL)
							{
								pch++;
								strcpy(message,pch);
								message[strlen(message)-1]='\0';
								for(j=0;j<=maxi;j++)
								{
									if(FD_ISSET(client[j].fd,&allset))
									{
										memset(buffer,0,sizeof(buffer));
										snprintf(buffer,sizeof(buffer),"[Server] %s yell %s\n",client[i].name,message);
										write(client[j].fd,buffer,strlen(buffer));
									}
								}		
							}
							else
							{
								memset(buffer,0,sizeof(buffer));
								snprintf(buffer,sizeof(buffer),"[Server] ERROR: Error command.\n",client[i].name,message);						
								write(sockfd,buffer,strlen(buffer));
							}
						}
						else if(strcmp(command,"tell")==0)
						{
							char* pch;
							pch = strchr(buffer,' ');
							if(pch!=NULL)
							{
								pch = strchr(pch+1,' ');
								if(pch!=NULL)
								{
									pch++;
									strcpy(message,pch);							
									message[strlen(message)-1]='\0';
									pch = strchr(buffer,' ');
									pch++;							
									strcpy(tellperson,pch);
									pch=strchr(tellperson,' ');
									*pch='\0';
									int notexist=1;
									for(j=0;j<=maxi;j++)
									{
										if(strcmp(client[j].name,tellperson)==0)
										{
											notexist=0;
											break;
										}
									}
									if(strcmp(client[i].name,"anonymous")==0)
									{
										memset(buffer,0,sizeof(buffer));
										snprintf(buffer,sizeof(buffer),"[Server] ERROR: You are anonymous.\n");
										write(client[i].fd,buffer,strlen(buffer));	
									}
									else if(strcmp(tellperson,"anonymous")==0)
									{
										memset(buffer,0,sizeof(buffer));
										snprintf(buffer,sizeof(buffer),"[Server] ERROR: The client to which you sent is anonymous.\n");
										write(client[i].fd,buffer,strlen(buffer));							
									}
									else if(notexist)
									{
										memset(buffer,0,sizeof(buffer));
										snprintf(buffer,sizeof(buffer),"[Server] ERROR: The receiver doesn't exist.\n");
										write(client[i].fd,buffer,strlen(buffer));									
									}
									else
									{
										memset(buffer,0,sizeof(buffer));
										snprintf(buffer,sizeof(buffer),"[Server] SUCCESS: Your message has been sent.\n");
										write(client[i].fd,buffer,strlen(buffer));								
										memset(buffer,0,sizeof(buffer));
										snprintf(buffer,sizeof(buffer),"[Server] %s tell you %s\n",client[i].name,message);
										write(client[j].fd,buffer,strlen(buffer));
									}
								}
								else
								{
									memset(buffer,0,sizeof(buffer));
									snprintf(buffer,sizeof(buffer),"[Server] ERROR: Error command.\n",client[i].name,message);						
									write(sockfd,buffer,strlen(buffer));
								}
							}
							else
							{						
								memset(buffer,0,sizeof(buffer));
								snprintf(buffer,sizeof(buffer),"[Server] ERROR: Error command.\n",client[i].name,message);						
								write(sockfd,buffer,strlen(buffer));
							}
						}
						else
						{						
							memset(buffer,0,sizeof(buffer));
							snprintf(buffer,sizeof(buffer),"[Server] ERROR: Error command.\n",client[i].name,message);						
							write(sockfd,buffer,strlen(buffer));
						}
					}
					if (--nready <= 0)
						break;	
				}
			}
		}
}