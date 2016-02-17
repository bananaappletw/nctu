
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>


#define MAXLINE 256
#define SERV_PORT 53

typedef unsigned short bool;

#define false 0
#define true 1
#define mapsize 10
char* ipmap[mapsize]={"ptt.cc","bs2.to"};
char* ip[mapsize]={"140.113.168.8","140.112.172.2"};
typedef struct DNSheader{
	unsigned short Transaction_ID;
	bool QR_flag;
	unsigned short Opcode;
	bool AA_flag;
	bool TC_flag;
	bool RD_flag;
	bool RA_flag;
	unsigned short Response_code;
	unsigned short QDCount;
	unsigned short ANCount;
	unsigned short NSCount;
	unsigned short ARCount;
}Dheader;

typedef struct DNSpayload{
	char QName[MAXLINE];
	unsigned short QType;
	unsigned short QClass;
}Dpayload;

void print_header(Dheader header){
	printf("Transaction_ID = %d\n",header.Transaction_ID);
	printf("QR_flag = %d\n",header.QR_flag);
	printf("Opcode = %d\n",header.Opcode);
	printf("AA_flag = %d\n",header.AA_flag);
	printf("TC_flag = %d\n",header.TC_flag);
	printf("RD_flag = %d\n",header.RD_flag);
	printf("RA_flag = %d\n",header.RA_flag);
	printf("Response_code = %d\n",header.Response_code);
	printf("QDCount = %d\n",header.QDCount);
	printf("ANCount = %d\n",header.ANCount);
	printf("NSCount = %d\n",header.NSCount);
	printf("ARCount = %d\n",header.ARCount);
	fflush(stdout);
}

void print_payload(Dpayload payload){
	printf("QName = %s\n",payload.QName);
	printf("QType = %d\n",payload.QType);
	printf("QCLass = %d\n",payload.QClass);
	fflush(stdout);
}
/*
parse : parses the questions of incoming DNS query
arg mesg : the message of a DNS query
arg heder : the header portion of a DNS query
arg payload : the payload portion of a DNS query
returns a dictionary of parsed payload information
*/


void parse(char mesg[], Dheader *header ,Dpayload *payload){

	/*parsing DNS header*/
	header->Transaction_ID = (mesg[0] << 8) + mesg[1];
	header->QR_flag = (mesg[2] & 0x80) >> 7;
	header->Opcode = (mesg[2]& 0x70) >> 4;
	header->AA_flag = (mesg[2] & 0x04) >> 2;
	header->TC_flag = (mesg[2] & 0x02) >> 1;
	header->RD_flag = mesg[2] & 0x01;
	header->RA_flag = (mesg[3] & 0x80) >> 7;
	header->Response_code = (mesg[3] & 0x0F);
	header->QDCount = (mesg[4] << 8) + mesg[5];
	header->ANCount = (mesg[6] << 8) + mesg[7];
	header->NSCount = (mesg[8] << 8) + mesg[9];
	header->ARCount = (mesg[10] << 8) + mesg[11];

	/*parsing DNS payload*/
	int index = 12;
	int len = mesg[index];
	char addr[MAXLINE] = {0};
	while(len != 0){
		char tmp[MAXLINE] = {0};
		memcpy(tmp, mesg + index + 1, len);
		strcat(addr,tmp);
		index += len + 1;
		len = mesg[index];
		if(len != 0){
			strcat(addr,".");
		}
	}
	strcpy(payload->QName,addr);
	index += 1;
	payload->QType = (mesg[index] << 8) + mesg[index + 1];
	index += 2;
	payload->QClass = (mesg[index] << 8) + mesg[index + 1];

}

/*pack_header packs the dictionary which contains all the information and flags about the header into DNS header*/
void  pack_header(unsigned char* data,Dheader* header_info)
{
    int temp;
    data[0]=(unsigned char)(header_info->Transaction_ID>>8);
    data[1]=(unsigned char)(header_info->Transaction_ID&0xFF);
    temp=(header_info->QR_flag<<7)+(header_info->Opcode<<4)+
         (header_info->AA_flag<<2)+(header_info->TC_flag<<1)+
         (header_info->RD_flag);
    data[2]=(unsigned char)temp;
    temp=(header_info->RA_flag<<7)+(header_info->Response_code);
    data[3]=(unsigned char)temp;
    data[4]=(unsigned char)(header_info->QDCount>>8);
    data[5]=(unsigned char)(header_info->QDCount&0xFF);
    data[6]=(unsigned char)(header_info->ANCount>>8);
    data[7]=(unsigned char)(header_info->ANCount&0xFF);
    data[8]=(unsigned char)(header_info->NSCount>>8);
    data[9]=(unsigned char)(header_info->NSCount&0xFF);
    data[10]=(unsigned char)(header_info->ARCount>>8);
    data[11]=(unsigned char)(header_info->ARCount&0xFF);
    data[12]='\0';
}


/*pack_resource_record packs the mapped ip address and TLL into DNS resource record*/
void pack_resource_record(unsigned char* data,char* ip,int TTL)
{
    char tok[16];
    strcpy(tok,ip); 
    data[0]=(unsigned char)0xC0;
    data[1]=(unsigned char)0x0C;
    data[2]=(unsigned char)0x00;
    data[3]=(unsigned char)0x01;
    data[4]=(unsigned char)0x00;
    data[5]=(unsigned char)0x01;
    data[6]=(unsigned char)((TTL >> 24) & 0xFF);
    data[7]=(unsigned char)((TTL >> 16) & 0xFF);
    data[8]=(unsigned char)((TTL >> 8) & 0xFF);
    data[9]=(unsigned char)(TTL & 0xFF);
    data[10]=(unsigned char)0x00;
    data[11]=(unsigned char)0x04;
    data[12]=(unsigned char)atoi(strtok(tok,"."));
    data[13]=(unsigned char)atoi(strtok(NULL,"."));
    data[14]=(unsigned char)atoi(strtok(NULL,"."));
    data[15]=(unsigned char)atoi(strtok(NULL,"."));
    data[16]='\0';
}

char* findip(char* domain_name)
{
    int i=0;
    for(;i<mapsize;i++)
    {
        if(strcmp(ipmap[i],domain_name)==0)
            return ip[i];
    }
    return "";
}

void recv_msg(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen)
{
	int n;
	int TTL=30;
	socklen_t len;
	unsigned char mesg[MAXLINE];
	for(;;)
	{
		len = clilen;
		/* waiting for receive data */
		n = recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
		unsigned char packet[MAXLINE]="";
		unsigned char response_header[MAXLINE]="";
		unsigned char response_rr[MAXLINE]="";
		Dheader header;
		Dpayload payload;
		parse(mesg,&header,&payload);		
		print_header(header);
		print_payload(payload);
		if(header.QR_flag==0&&payload.QType==0x01)
		{
		    printf("Got a DNS query from %d : %s\n",sockfd,payload.QName);
	            char* tempip="";
		    tempip=findip(payload.QName);
		    if(strcmp(tempip,""))
		    {
		        printf("\tFound! Reply : %s -> %s\n",payload.QName,tempip);

			//set the Query/Response flag to response
			header.QR_flag = 1;

			//set Recursion Available flag to 1
			header.RA_flag = 1;

			//set Answer Count to 1
			header.ANCount = 1;
						

			//pack a resource record according to the question name
			pack_resource_record(response_rr,tempip, TTL);

			//if the question name is NOT found in out map
		    }
		    else
		    {
		        printf("\tNot found. Reply : No such name.\n");


			//set the Query/Response flag to response
			header.QR_flag = 1;

			//set the Response Code to "Name Error" (NXDOMAIN)
			header.Response_code = 0x03;
		    }
			//create a response packet and send
			pack_header(response_header,&header);
			memcpy(packet,response_header,12);			
			//strcat(packet,response_header);
			memcpy(packet+12,mesg+12,n-12);
			//strcat(packet,addpayload);
			memcpy(packet+24,response_rr,16);
			//strcat(packet,response_rr);
			// sent data back to client
			/*packet[40]='\0'; 
			int j=0;
			for(;j<40;j++)
			printf("%d ",packet[j]);

			fflush(stdout);*/
			sendto(sockfd, packet, 40, 0, pcliaddr, clilen);		
		}
	}
}
int main(void)
{
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0); /* create a UDP socket */

	/* init servaddr */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	/* bind address and port to socket */
	if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{
		perror("bind error");
		exit(1);
	}
	recv_msg(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

	return 0;
}
