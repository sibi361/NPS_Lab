#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <math.h>
#define PORT 8080
#define MAXLINE 1024

void hammingCalculation(int data[],int hammingCode[])
{
             	hammingCode[0]=data[0];
             	hammingCode[1]=data[1];
             	hammingCode[2]=data[2];
             	hammingCode[4]=data[3];
            	hammingCode[3]=data[0]^data[1]^data[2];
	        hammingCode[5]=data[0]^data[1]^hammingCode[4];  
	        hammingCode[6]=data[0]^data[2]^hammingCode[4]; 
	     	printf("the encoded bits are: ");
	    	for (int i=0;i<7;i++) 
    		{
				printf("%d ",hammingCode[i]);
	  		}
			printf("\n");
}

int main() 
{
			int sockfd;
			struct sockaddr_in servaddr, cliaddr;
			if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
   			{
   				perror("socket creation failed!!");
   				exit(EXIT_FAILURE);
   			}
			memset(&servaddr, 0, sizeof(servaddr));
			memset(&cliaddr, 0, sizeof(cliaddr));
			servaddr.sin_family = AF_INET; 
			servaddr.sin_addr.s_addr = INADDR_ANY;
			servaddr.sin_port = htons(PORT);
			if ( bind(sockfd, (const struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 )
   			{
   			    perror("bind failed!!");
   			    exit(EXIT_FAILURE);
   			}
			printf("server waiting...\n");
			int len;
			len = sizeof(cliaddr); //len is value/result
			int size;
			recvfrom(sockfd,&size,sizeof(int),MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
			int receivedBits[size];
			int sentBits[7];
			int sizeofCode=7;
			recvfrom(sockfd,&receivedBits,size*sizeof(int),MSG_WAITALL, (struct sockaddr *) &servaddr,&len);
			printf("data received...\n");
			printf("performing operation...\n");
			hammingCalculation(receivedBits,sentBits);
			sendto(sockfd,&sizeofCode,sizeof(int),MSG_CONFIRM, (const struct sockaddr *) &servaddr,sizeof(servaddr));
			sendto(sockfd,&sentBits,sizeofCode*sizeof(int),MSG_CONFIRM, (const struct sockaddr *) &servaddr,sizeof(servaddr));
			printf("data sent...\n");
			return 0;
}
