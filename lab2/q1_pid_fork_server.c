#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#define MAXSIZE 90

void main()
{
	int sockfd, newsockfd, retval, i;
	socklen_t actuallen;
	int recedbytes, sentbytes;
	struct sockaddr_in serveraddr, clientaddr;
	char buff[MAXSIZE];

	int a = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1)
	{
		printf("\nSocket creation error\n");
	}

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(3388);
	serveraddr.sin_addr.s_addr = htons(INADDR_ANY);
	retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (retval == 1)
	{
		printf("Binding error\n");
		close(sockfd);
		exit(0);
	}

	retval = listen(sockfd, 1);
	if (retval == -1)
	{
		close(sockfd);
		exit(0);
	}
	actuallen = sizeof(clientaddr);

	if (newsockfd == -1)
	{
		close(sockfd);
		exit(0);
	}

	int pid;

	newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);

	while (1)
	{
		memset(buff, '\0', sizeof(buff));

		recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
		if (recedbytes == -1 || !buff[0] || !strcmp(buff, "stop"))
		{
			close(newsockfd);
			close(sockfd);
			exit(0);
		}

		pid = fork();
		if (pid == 0) /* child process */
		{

			printf("PID child: %d\nreceived %s \n", pid, buff);
		}
		else
		{
			memset(buff, '\0', sizeof(buff));
			printf("\nPID parent: %d\n", pid);
			scanf("%s", buff);
			buff[strlen(buff)] = '\0';
			int buff_size = strlen(buff) * sizeof(char);
			sentbytes = send(newsockfd, buff, buff_size, 0);

			if (sentbytes == -1 || !strcmp(buff, "stop"))
			{
				close(sockfd);
				close(newsockfd);
				exit(0);
			}
		}
	}
	close(sockfd);
	close(newsockfd);
}
