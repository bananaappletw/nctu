#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<unistd.h>
struct sigaction sigchld_action={
.sa_handler = SIG_DFL,
.sa_flags = SA_NOCLDWAIT};
int main()
{
	int i,background;
	char buffer [1024];
	char cur_dir[256];
	char home_dir[256];
	getcwd(home_dir,256);
	char* pch;
	char* parameter[128];
	sigaction(SIGCHLD, &sigchld_action,NULL);
	while(1)
	{
		printf(">");
		fgets(buffer,1024,stdin);
		buffer[strlen(buffer)-1]='\0';
		i=0;
		background=0;
		pch = strtok(buffer," ");	
		while(pch != NULL)
		{
			parameter[i]=pch;
			pch = strtok(NULL," ");	
			i++;
		}	
		if(strcmp(parameter[i-1],"&")==0)
		{
			background=1;
			i--;
		}
		parameter[i]=NULL;
		if(strcmp(parameter[0],"exit")==0)
			break;
		else if(strcmp(parameter[0],"cd")==0)
		{
			if(parameter[1]==NULL)
			{
				chdir(home_dir);
			}

			else if(parameter[1][0]=='~')
			{
				char temp_dir[256];
				strcpy(temp_dir,home_dir);
				getcwd(cur_dir,256);
				memcpy(&temp_dir[strlen(temp_dir)],&parameter[1][1],strlen(cur_dir));
				chdir(temp_dir);	
			}
			else 
				chdir(parameter[1]);	
		}
		else	
		{
			pid_t pid;
			pid=fork();
			if(pid<0)
			{
				printf("fork error\n");
			}
			else if(pid == 0)
			{
				execvp(parameter[0],parameter);	
				exit(1);
			}
			else
			{
				if(!background)
					waitpid(pid,NULL,0);		
				else
					waitpid(pid,NULL,WNOHANG);	
					
			}
		}
	
	}
	return 0;
}	

