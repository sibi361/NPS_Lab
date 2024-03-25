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
	int sockfd, newsockfd, retval, len, swapped, swap_tmp;
	socklen_t actuallen;
	int recedbytes, sentbytes;
	struct sockaddr_in serveraddr, clientaddr;
	char buff[MAXSIZE], buff2[MAXSIZE];

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
		if (recedbytes == -1)
		{
			printf("Receive error\n");
			close(newsockfd);
			close(sockfd);
			exit(0);
		}
		else if (!buff[0])
		{
			continue;
		}

		printf("\nReceived: %s\n", buff);

		pid = fork();
		if (pid == 0) /* child process */
		{
			len = strlen(buff);
			char numbers[len];
			int j = 0;

			for (int i = 0; i < len; i++)
			{
				if (buff[i] >= '0' && buff[i] <= '9')
					numbers[j++] = buff[i];
			}
			numbers[j] = '\0';

			len = strlen(numbers);
			for (int i = 0; i < len - 1; i++)
			{
				swapped = 0;

				for (int j = 0; j < len - 1 - i; j++)
				{
					if (numbers[j] > numbers[j + 1])
					{
						swap_tmp = numbers[j];
						numbers[j] = numbers[j + 1];
						numbers[j + 1] = swap_tmp;
						swapped = 1;
					}
				}

				if (!swapped)
				{
					break;
				}
			}

			printf("\nOutput at the child process of the server: %s\n", numbers);
			char output[] = "Output at the child process of the server: ";
			strcat(output, numbers);
			strcpy(buff, output);
			strcat(buff2, "\n\0");

			int buff_size = strlen(buff) * sizeof(char);
			sentbytes = send(newsockfd, buff, sizeof(buff2), 0);
			if (sentbytes == -1)
			{
				printf("Sending error\n");
				close(sockfd);
				exit(0);
			}
		}
		else
		{
			len = strlen(buff);
			char letters[len];
			int j = 0;

			for (int i = 0; i < len; i++)
			{
				if ((buff[i] >= 'a' && buff[i] <= 'z') || (buff[i] >= 'A' && buff[i] <= 'Z'))
					letters[j++] = buff[i];
			}
			letters[j] = '\0';

			len = strlen(letters);
			for (int i = 0; i < len - 1; i++)
			{
				swapped = 0;

				for (int j = 0; j < len - 1 - i; j++)
				{
					if (letters[j] > letters[j + 1])
					{
						swap_tmp = letters[j];
						letters[j] = letters[j + 1];
						letters[j + 1] = swap_tmp;
						swapped = 1;
					}
				}

				if (!swapped)
				{
					break;
				}
			}

			printf("\nOutput at the parent process of the server: %s\n", letters);
			char output[] = "Output at the parent process of the server: ";
			strcat(output, letters);
			strcpy(buff2, output);
			strcat(buff2, "\n\0");

			int buff_size = strlen(buff2) * sizeof(char);
			sentbytes = send(newsockfd, buff2, sizeof(buff2), 0);
			if (sentbytes == -1)
			{
				printf("Sending error\n");
				close(sockfd);
				exit(0);
			}
		}
	}
	close(sockfd);
	close(newsockfd);
}
