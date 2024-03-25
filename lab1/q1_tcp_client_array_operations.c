// Q1 TCP program to send a numeric array to the server and perform operations
// CLIENT

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
#define LISTEN_QUEUE_LEN 100

#define FAIL_SERVER_CONNECT_SLEEP_SECONDS 0.1
#define FAIL_SLEEP_SECONDS 2
#define FAIL_MAX 10

int sockfd, retval, listeningPort = PORT_SERVER;

void exitClient()
{
	printf("\n\nKeyboardInterrupt, exiting\n");
	close(sockfd);
	exit(0);
}

void connectToServer(int *retval, int *failCount, struct sockaddr_in serveraddr)
{
	while (1)
	{
		serveraddr.sin_port = htons(listeningPort);

		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
		{
			printf("\nSocket Creation Error\n");
			if (*failCount > FAIL_MAX)
			{
				printf("\nFailed too many times, exiting\n");
				return;
			}
			sleep(FAIL_SLEEP_SECONDS);
			*failCount += 1;
		}

		*retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
		if (*retval == -1)
		{
			listeningPort += 1;

			printf("\nConnection error\nTrying port %d\n", listeningPort);

			if (*failCount > FAIL_MAX)
			{
				printf("\nFailed too many times, exiting\n");
				close(sockfd);
				return;
			}
			sleep(FAIL_SERVER_CONNECT_SLEEP_SECONDS);
			*failCount += 1;
		}
		else
		{
			// printf("\nCLIENT connected to server at port %d\n", PORT_SERVER);
			return;
		}
	}
}

void main()
{
	signal(SIGINT, exitClient);

	int retval, failCount, choice, len, tmp;
	socklen_t actuallen;
	int receivedBytes, sentBytes;
	struct sockaddr_in serveraddr;
	char buffer[MAXSIZE];

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1\n");

	// connectToServer(&retval, &failCount, serveraddr);

	while (1)
	{
		connectToServer(&retval, &failCount, serveraddr);

		memset(buffer, '\0', sizeof(buffer));

		printf("\nEnter the no of elements in array: ");
		scanf("%d", &len);
		printf("\nEnter the elements in array: ");
		for (int i = 0; i < len; i++)
			scanf("%d", &buffer[i]);

		buffer[strlen(buffer)] = '\0';

		int buff_size = strlen(buffer) * sizeof(char);
		send(sockfd, buffer, buff_size, 0);

		printf("\n\n1. Search");
		printf("\n2. Sort in ascending order");
		printf("\n3. Sort in descending order");
		printf("\n4. Exit");
		printf("\nEnter your choice: ");
		scanf("%d", &choice);

		send(sockfd, &len, sizeof(len), 0);
		send(sockfd, &choice, sizeof(choice), 0);

		switch (choice)
		{
		case 1:
			printf("\nEnter the no to search for: ");
			scanf("%d", &tmp);
			send(sockfd, &tmp, sizeof(tmp), 0);
			recv(sockfd, &tmp, sizeof(tmp), 0);

			if (tmp == -1)
			{
				printf("Search query NOT found in array\n");
			}
			else
			{
				printf("Search query found at index %d\n", tmp);
			}

			break;
		case 2:
			recv(sockfd, buffer, sizeof(buffer), 0);

			for (int i = 0; i < len; i++)
			{
				printf("%d ", buffer[i]);
			}
			printf("\n");

			break;
		case 3:
			recv(sockfd, buffer, sizeof(buffer), 0);

			for (int i = 0; i < len; i++)
			{
				printf("%d ", buffer[i]);
			}
			printf("\n");

			break;
		case 4:
			close(sockfd);
			exit(0);
			break;
		default:
			printf("invalid choice");
			continue;
		}
	}

	close(sockfd);
}
