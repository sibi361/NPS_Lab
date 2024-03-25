// Q2 UDP string permutations SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

#define MAXSIZE 90
#define PORT_LISTENING 2345
#define PORT_LISTENING_CLIENT 3345
#define LISTEN_QUEUE_LEN 100

#define FAIL_SLEEP_SECONDS 1
#define FAIL_MAX 100

void swap(char *a, char *b)
{
    char temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

void permute(char *input_string, int left_idx, int right_idx)
{
    int i;
    if (left_idx == right_idx)
        printf("%s\n", input_string);
    else
    {
        for (i = left_idx; i <= right_idx; i++)
        {
            swap((input_string + left_idx), (input_string + i));
            permute(input_string, left_idx + 1, right_idx);
            swap((input_string + left_idx), (input_string + i));
        }
    }
}

void main()
{
    socklen_t actuallen;
    int sockfd, newsockfd;
    int failCount, retval, receivedBytes, sentBytes, len;
    int listeningPort, listeningPortGiven;
    struct sockaddr_in serveraddr, clientaddr;
    char buffer[MAXSIZE];

    listeningPort = PORT_LISTENING;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        printf("\nSocket creation error\n");
        exit(0);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(listeningPort);
    serveraddr.sin_addr.s_addr = htons(INADDR_ANY);

    clientaddr.sin_family = AF_INET;
    clientaddr.sin_port = htons(PORT_LISTENING_CLIENT);
    clientaddr.sin_addr.s_addr = htons(INADDR_ANY);
    actuallen = sizeof(clientaddr);

    retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1)
    {
        printf("\nBinding error\n");
        close(sockfd);
        exit(0);
    }

    while (1)
    {
        memset(buffer, '\0', sizeof(buffer));

        receivedBytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientaddr, &actuallen);
        if (receivedBytes == -1)
        {
            printf("\nReceive error\n");
            sleep(2);
        }
        else if (!strcmp(buffer, "halt"))
        {
            printf("\nReceived STOP. Exiting\n");
            close(sockfd);
            exit(0);
        }

        printf("Received: %s \n", buffer);

        len = strlen(buffer);

        permute(buffer, 0, len - 1);
        printf("\n");
    }

    close(sockfd);
}
