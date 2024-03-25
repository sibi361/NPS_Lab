// Q1 TCP program to send a numeric array to the server and perform operations
// SERVER

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
#define LISTEN_QUEUE_LEN 100

#define FAIL_SLEEP_SECONDS 1
#define FAIL_MAX 100

int sockfd, newsockfd;

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

    socklen_t actuallen;
    int failCount, retval, receivedBytes, sentBytes;
    int listeningPort, listeningPortGiven, pid;
    int choice, len, tmp, swap_tmp, swapped, found;
    struct sockaddr_in serveraddr, clientaddr;
    char buffer[MAXSIZE];

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
        actuallen = sizeof(clientaddr);

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
            /*if (failCount > FAIL_MAX)
            {
                printf("\nFailed too many times, exiting\n");
                return;
            }
            sleep(FAIL_SLEEP_SECONDS);
            failCount += 1;*/
            listeningPort += 1;
        }
    }

    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);
        if (newsockfd == -1)
        {
            continue;
        }
        // printf("\nReceived connection from client on fd: %d\n", newsockfd);

        memset(buffer, '\0', sizeof(buffer));

        receivedBytes = recv(newsockfd, buffer, sizeof(buffer), 0);
        if (receivedBytes == -1 || !buffer[0])
        {
            printf("\nReceive error\n");
            sleep(2);
            continue;
        }

        recv(newsockfd, &len, sizeof(len), 0);
        recv(newsockfd, &choice, sizeof(choice), 0);

        printf("\nChoice: %d", choice);
        switch (choice)
        {
        case 1:
            recv(newsockfd, &tmp, sizeof(tmp), 0);

            found = -1;
            for (int i = 0; i < len; i++)
            {
                if (buffer[i] == tmp)
                {
                    printf("\nFound search query %d at index: %d\n", tmp, i);
                    found = i;
                    break;
                }
            }
            send(newsockfd, &found, sizeof(found), 0);
            break;
        case 2:
            for (int i = 0; i < len - 1; i++)
            {
                swapped = 0;

                for (int j = 0; j < len - 1 - i; j++)
                {
                    if (buffer[j] > buffer[j + 1])
                    {
                        swap_tmp = buffer[j];
                        buffer[j] = buffer[j + 1];
                        buffer[j + 1] = swap_tmp;
                        swapped = 1;
                    }
                }

                if (!swapped)
                {
                    break;
                }
            }

            send(newsockfd, buffer, sizeof(buffer), 0);

            break;
        case 3:
            for (int i = 0; i < len - 1; i++)
            {
                swapped = 0;

                for (int j = 0; j < len - 1 - i; j++)
                {
                    if (buffer[j] < buffer[j + 1])
                    {
                        swap_tmp = buffer[j];
                        buffer[j] = buffer[j + 1];
                        buffer[j + 1] = swap_tmp;
                        swapped = 1;
                    }
                }

                if (!swapped)
                {
                    break;
                }
            }

            send(newsockfd, buffer, sizeof(buffer), 0);

            break;
        default:
            printf("\ninvalid choice");
            continue;
        }
    }

    close(sockfd);
    close(newsockfd);
}
