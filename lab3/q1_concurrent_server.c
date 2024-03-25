#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

#define MAXSIZE 90
#define PORT_LISTENING 8046
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

    socklen_t actuallen, counter = 0;
    int failCount, retval, receivedBytes, sentBytes, listeningPort, listeningPortGiven;
    struct sockaddr_in serveraddr;
    char buffer[MAXSIZE];

    listeningPort = PORT_LISTENING;

    FILE *f = fopen("q1_output.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(f, "%s\n", buffer);
    fclose(f);

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

        // if (counter >= 3)
        // {
        //     printf("Terminating as number of connections exceeds 2\n");

        //     strcpy(buffer, "terminate session\0");
        //     send(newsockfd, buffer, sizeof(buffer), 0);

        //     close(newsockfd);
        //     close(sockfd);
        //     exit(0);
        // }

        socklen_t len;
        struct sockaddr_storage addr;
        char ipstr[INET6_ADDRSTRLEN];
        int port;

        len = sizeof(addr);
        getpeername(newsockfd, (struct sockaddr *)&serveraddr, &actuallen);

        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        port = ntohs(s->sin_port);
        inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);

        printf("Socket address of client: %s:%d\n", ipstr, port);

        /////

        int pid = fork();

        if (pid)
        {
            continue;
        }

        while (1)
        {
            memset(buffer, '\0', sizeof(buffer));

            receivedBytes = recv(newsockfd, buffer, sizeof(buffer), 0);
            if (receivedBytes == -1)
            {
                printf("\nReceive error\n");
                sleep(2);
            }

            printf("Received %s", buffer);

            FILE *f = fopen("q1_output.txt", "a");
            if (f == NULL)
            {
                printf("Error opening file!\n");
                exit(1);
            }

            fprintf(f, " %s ", buffer);

            fclose(f);

            break;
        }
    }

    close(sockfd);
    close(newsockfd);
}
