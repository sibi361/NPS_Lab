// Q2 DNS Server
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

#define MAXSIZE 200
#define PORT_SERVER 2345
#define LISTEN_QUEUE_LEN 100

#define FAIL_SERVER_CONNECT_SLEEP_SECONDS 0.1
#define FAIL_SLEEP_SECONDS 2
#define FAIL_MAX 10

#define LEAK_RATE 1
#define BUCKET_SIZE 10
#define PACKET_SIZE 4

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

	int arrival_times[] = {1, 2, 3, 5, 6, 8, 11, 12, 15, 16, 19};
	int num_packets = sizeof(arrival_times) / sizeof(arrival_times[0]);

	// connectToServer(&retval, &failCount, serveraddr);

	for (int i = 0; i < num_packets; i++)
	{
		connectToServer(&retval, &failCount, serveraddr);

		// printf("%d\n", arrival_times[i]);
		send(sockfd, &(arrival_times[i]), sizeof(arrival_times[i]), 0);

		memset(buffer, '\0', sizeof(buffer));
		recv(sockfd, buffer, sizeof(buffer), 0);
		printf("Status for packet %d at time %d: %s\n", i + 1, arrival_times[i], buffer);
	}

	close(sockfd);
}
