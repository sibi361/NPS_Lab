#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

#define MAXSIZE 90
#define PORT_LISTENING 8056
#define LISTEN_QUEUE_LEN 100

#define FAIL_SLEEP_SECONDS 1
#define FAIL_MAX 100

int sockfd, newsockfd;

int checkAnagram(char *str1, char *str2)
{
    int count1[MAXSIZE] = {0};
    int count2[MAXSIZE] = {0};
    int i;

    for (i = 0; str1[i] && str2[i] && str1[i] != '\0' && str2[i] != '\0'; i++)
    {
        count1[str1[i]]++;
        count2[str2[i]]++;
    }

    if (str1[i] || str2[i])
        return 0;

    for (i = 0; i < MAXSIZE; i++)
        if (count1[i] != count2[i])
            return 0;

    return 1;
}

void exitServer()
{
    printf("\n\nKeyboardInterrupt, exiting\n");
    close(sockfd);
    close(newsockfd);
    exit(0);
}

void main()
{
    signal(SIGINT, exitServer);

    socklen_t actuallen, counter = 0, result;
    int failCount, retval, receivedBytes, sentBytes, listeningPort, listeningPortGiven;
    struct sockaddr_in serveraddr;
    char buffer[MAXSIZE], buffer2[MAXSIZE];

    listeningPort = PORT_LISTENING;

    while (1)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd == -1)
        {
            printf("\nSocket creation error\n");
        }

        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(listeningPort);
        serveraddr.sin_addr.s_addr = htons(INADDR_ANY);

        retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
        if (retval == 1)
        {
            printf("\nBinding error\n");
            close(sockfd);
            exit(0);
        }

        retval = listen(sockfd, LISTEN_QUEUE_LEN);
        if (retval == -1)
        {
            close(sockfd);
            exit(0);
        }
        actuallen = sizeof(serveraddr);

        socklen_t len = sizeof(serveraddr);
        if (getsockname(sockfd, (struct sockaddr *)&serveraddr, &len) == -1)
        {
            perror("getsockname");
            printf("\nSocket Creation Error\n");
            if (failCount > FAIL_MAX)
            {
                printf("\nFailed too many times, exiting\n");
                return;
            }
            sleep(FAIL_SLEEP_SECONDS);
            failCount += 1;
        }
        else
        {
            listeningPortGiven = ntohs(serveraddr.sin_port);
            if (listeningPortGiven == listeningPort)
            {
                printf("\nSERVER listening on port %d\n", ntohs(serveraddr.sin_port));
                break;
            }
            printf("\nSocket Creation Error, wrong port allocated: %d\n", listeningPortGiven);
            listeningPort += 1;
        }
    }

    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&serveraddr, &actuallen);
        if (newsockfd == -1)
        {
            printf("accept error\n");
            continue;
        }
        // printf("\nReceived connection from client on fd: %d\n", newsockfd);

        counter += 1;
        printf("\nClients connected: %d\n", counter);

        // if (counter == 3)c

        while (1)
        {
            memset(buffer, '\0', sizeof(buffer));

            receivedBytes = recv(newsockfd, buffer, sizeof(buffer), 0);
            if (receivedBytes == -1)
            {
                printf("\nReceive error\n");
                continue;
            }

            memset(buffer2, '\0', sizeof(buffer2));

            receivedBytes = recv(newsockfd, buffer2, sizeof(buffer2), 0);
            if (receivedBytes == -1)
            {
                printf("\nReceive error\n");
                continue;
            }

            printf("string1: %s\n", buffer);
            printf("string2: %s\n", buffer2);

            result = checkAnagram(buffer, buffer2);
            if (result)
            {
                printf("The given strings are anagrams\n");
            }
            else
            {
                printf("The given strings are NOT anagrams\n");
            }

            send(newsockfd, &result, sizeof(result), 0);

            break;
        }
    }

    close(sockfd);
    close(newsockfd);
}
