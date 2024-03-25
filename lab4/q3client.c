#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define PORT 8080
#define MAXLINE 1024

int main() 
{
      	int sockfd;
      	char buffer[MAXLINE];
      	struct sockaddr_in servaddr;

      	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
      	{
         	perror("socket creation failed!!!");
         	exit(EXIT_FAILURE);
      	}
      	memset(&servaddr, 0, sizeof(servaddr));

      	servaddr.sin_family = AF_INET;
      	servaddr.sin_port = htons(PORT);
      	servaddr.sin_addr.s_addr = INADDR_ANY;
      	printf("connected!!\n");

      	int n=4;
/*printf("Please enter the size of Dataword: ");
	scanf("%d",&n);*/
	    int arr[n];
	    printf("Please enter the %d  bit size data(each bit separated by a space): ",n);
	    for (int i = 0; i <n; i++) 
        {
	      	scanf("%d",&arr[i]);
	    }
	    
      	sendto(sockfd,&n,sizeof(int),MSG_CONFIRM, (const struct sockaddr *) &servaddr,sizeof(servaddr));
      	sendto(sockfd,&arr,n*sizeof(int),MSG_CONFIRM, (const struct sockaddr *) &servaddr,sizeof(servaddr));
      	printf("data sent...\n");
      	printf("waiting for the server...\n");

     	int size,len;
     	recvfrom(sockfd,&size,sizeof(int),MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
     	int receivedBits[size];
     	recvfrom(sockfd,&receivedBits,size*sizeof(int),MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
     	printf("data received...\n");

     	for(int i=0;i<size;i++)
     	{
        	printf("%d ",receivedBits[i]);
     	}
     	printf("\n");
     	close(sockfd);
     	return 0;
}
