// Q2 UDP palindrome checker server

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

void main()
{
    socklen_t actuallen;
    int sockfd, newsockfd;
    int failCount, retval, receivedBytes, sentBytes, len;
    int listeningPort, listeningPortGiven, palindrome;
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
        palindrome = 1;

        for (int i = 0; i < len / 2; i++)
        {
            if (buffer[i] != buffer[len - i - 1])
            {
                palindrome = 0;
                break;
            }
        }

        if (palindrome)
        {
            printf("Given string is a palindrome\n");
        }
        else
        {
            printf("Given string is NOT a palindrome\n");
        }

        /////

        sentBytes = sendto(sockfd, palindrome, sizeof(palindrome), 0, (struct sockaddr *)&clientaddr, actuallen);
        if (sentBytes == -1)
        {
            printf("\nSending failed. Try again\n");
        }
        else if (!strcmp(buffer, "halt"))
        {
            printf("\nReceived STOP. Exiting\n");
            close(sockfd);
            exit(0);
        }
    }

    close(sockfd);
}
