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
#define MAXSIZE 50

void main()
{
	char buff[MAXSIZE];
	int sockfd, retval, i;
	int recedbytes, sentbytes;
	struct sockaddr_in serveraddr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("\nSocket Creation Error\n");
		exit(0);
	}

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(3388);
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1\n");
	retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (retval == -1)
	{
		printf("Connection error\n");
		return;
	}

	while (1)
	{
		memset(buff, '\0', sizeof(buff));

		printf("\nEnter a string: ");
		// gets(buff);
		scanf("%s", buff);
		printf("\n\n");

		buff[strlen(buff)] = '\0';
		int buff_size = strlen(buff) * sizeof(char);

		sentbytes = send(sockfd, buff, buff_size, 0);
		if (sentbytes == -1 || !strcmp(buff, "stop") || !buff[0])
		{
			printf("Send error\n");
			close(sockfd);
			exit(0);
		}

		recedbytes = recv(sockfd, buff, sizeof(buff), 0);
		if (recedbytes == -1)
		{
			printf("Receive error\n");
			close(sockfd);
			exit(0);
		}
		else if (buff[0])
		{
			printf(buff);
		}

		recedbytes = recv(sockfd, buff, sizeof(buff), 0);
		if (recedbytes == -1)
		{
			printf("Receive error\n");
			close(sockfd);
			exit(0);
		}
		else if (!buff[0])
		{
			continue;
		}
		printf(buff);
	}
	// close(sockfd);
}
