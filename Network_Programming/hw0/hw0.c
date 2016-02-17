#include<stdio.h>
#include<stdlib.h>
#include<string.h>
void reverse(char* string)
{
    if(*string!='\0')
        reverse(string+1);
    printf("%c",*string);
}
int main(int argc,char **argv)
{
    if(argc !=3)
    {
        printf("usage: %s [input file] [spilt token]",argv[0]);
        exit(1);
    }
    char input_file[256];
    char* spilt_token;
    char* pch;
    char way[256];
    char buffer[256];
    strcpy(input_file,argv[1]);
    spilt_token=argv[2];
    FILE *fptr;
    fptr=fopen(input_file,"r");
    while(fscanf(fptr,"%s",way)!=EOF)
    {
        if(strcmp(way,"reverse")==0)
        {
            fscanf(fptr,"%s",buffer); 
            reverse(buffer);
            printf("\n");
            continue;
        }
        else if(strcmp(way,"spilt")==0)
        {
            fscanf(fptr,"%s",buffer); 
            pch=strtok(buffer,spilt_token);   
            while(pch!=NULL) 
            {
                printf("%s ",pch);
                pch=strtok(NULL,spilt_token);   
            }
            printf("\n");
            continue;
        }
        else if(strcmp(way,"exit")==0)
        {
            return 0;
        }
    }
    while(scanf("%s",way))
    {
        if(strcmp(way,"reverse")==0)
        {
            scanf("%s",buffer); 
            reverse(buffer);
            printf("\n");
            continue;
        }
        else if(strcmp(way,"spilt")==0)
        {
            scanf("%s",buffer); 
            pch=strtok(buffer,spilt_token);   
            while(pch!=NULL) 
            {
                printf("%s ",pch);
                pch=strtok(NULL,spilt_token);   
            }
            printf("\n");
            continue;
        }
        else if(strcmp(way,"exit")==0)
        {
            break;
        }
    }
    return 0;
}
