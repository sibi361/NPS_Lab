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
#define PORT_SERVER 8046
#define LISTEN_QUEUE_LEN 100

#define FAIL_SERVER_CONNECT_SLEEP_SECONDS 0.1
#define FAIL_SLEEP_SECONDS 2
#define FAIL_MAX 10

int sockfd, newsockfd, retval, listeningPort = PORT_SERVER;

void exitClient()
{
	printf("\n\nKeyboardInterrupt, exiting\n");
	close(sockfd);
	close(newsockfd);
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

	int newsockfd, retval, failCount;
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

		while (1)
		{
			memset(buffer, '\0', sizeof(buffer));
			printf("\nEnter message: ");
			gets(buffer);
			strcat(buffer, "\n");
			// fgets(buffer, MAXSIZE, stdin);
			buffer[strlen(buffer)] = '\0';

			int buff_size = strlen(buffer) * sizeof(char);
			sentBytes = send(sockfd, buffer, buff_size, 0);

			if (sentBytes == -1)
			{
				printf("\nSending failed. Try again\n");
				if (failCount > FAIL_MAX)
				{
					printf("\nFailed too many times, exiting\n");
					close(sockfd);
					exit(0);
				}
				close(sockfd);
				sleep(FAIL_SLEEP_SECONDS);
				connectToServer(&retval, &failCount, serveraddr);
			}
			else if (!strcmp(buffer, "stop"))
			{
				printf("\nExiting\n");
				close(sockfd);
				exit(0);
			}
			else
			{
				break;
			}
		}

		//////////

		while (1)
		{
			memset(buffer, '\0', sizeof(buffer));

			receivedBytes = recv(sockfd, buffer, sizeof(buffer), 0);
			if (receivedBytes == -1)
			{
				printf("\nReceive error\n");
				if (failCount > FAIL_MAX)
				{
					printf("\nFailed too many times, exiting\n");
					close(sockfd);
					exit(0);
				}
				close(sockfd);
				sleep(FAIL_SLEEP_SECONDS);
				connectToServer(&retval, &failCount, serveraddr);
			}
			else if (!buffer[0])
			{
				continue;
			}

			printf("\nReceived: %s \n", buffer);
			break;
		}
	}

	close(sockfd);
}
