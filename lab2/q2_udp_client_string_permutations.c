// Q2 UDP string permutations CLIENT

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>

#define MAXSIZE 90
#define PORT_SERVER 2345
#define PORT_LISTENING_CLIENT 3345
#define LISTEN_QUEUE_LEN 100

#define FAIL_SERVER_CONNECT_SLEEP_SECONDS 0.1
#define FAIL_SLEEP_SECONDS 2
#define FAIL_MAX 10

void main()
{
	int sockfd, listeningPort = PORT_SERVER;
	int retval, failCount, palindrome;
	socklen_t actuallen;
	int receivedBytes, sentBytes;
	struct sockaddr_in serveraddr, clientaddr;
	char buffer[MAXSIZE];

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(listeningPort);
	serveraddr.sin_addr.s_addr = htons(INADDR_ANY);

	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(PORT_LISTENING_CLIENT);
	clientaddr.sin_addr.s_addr = htons(INADDR_ANY);
	actuallen = sizeof(serveraddr);

	while (1)
	{
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sockfd == -1)
		{
			printf("\nSocket Creation Error\n");
			exit(0);
		}

		retval = bind(sockfd, (struct sockaddr *)&clientaddr, &actuallen);
		if (sentBytes == -1)
		{
			printf("\nBinding failed\n");
			close(sockfd);
			exit(0);
		}

		memset(buffer, '\0', sizeof(buffer));
		printf("\nEnter message: ");
		scanf("%s", buffer);
		buffer[strlen(buffer)] = '\0';

		sentBytes = sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&serveraddr, actuallen);
		if (sentBytes == -1)
		{
			printf("\nSending failed\n");
			close(sockfd);
			exit(0);
		}
		else if (!strcmp(buffer, "halt"))
		{
			printf("\nExiting\n");
			close(sockfd);
			exit(0);
		}
	}

	close(sockfd);
}
