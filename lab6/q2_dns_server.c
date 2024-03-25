// Q2 DNS Server
// SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

#define MAXSIZE 200
#define DOMAIN_MAXLEN 100
#define IP_MAXLEN 16

#define PORT_LISTENING 2345
#define LISTEN_QUEUE_LEN 100

#define FAIL_SLEEP_SECONDS 1
#define FAIL_MAX 100

struct domain_ip
{
    char domain[DOMAIN_MAXLEN], ip[IP_MAXLEN];
};

struct domain_ip database[] = {{"google.com", "142.250.80.14"},
                               {"microsoft.com", "20.112.250.133"},
                               {"timesofindia.com", "23.209.72.236"},
                               {"yahoo.com", "98.137.11.163"},
                               {"example.com", "93.184.216.34"}};
int database_len = sizeof(database) / sizeof(database[0]);

int sockfd,
    newsockfd;

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
    int failCount, retval, receivedBytes, sentBytes, found;
    int listeningPort, listeningPortGiven, pid;
    struct sockaddr_in serveraddr, clientaddr;
    char buffer[MAXSIZE], temp[MAXSIZE];

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

        buffer[strlen(buffer) - 1] = '\0';
        printf("\nReceived: %s", buffer);

        found = 0;

        for (int i = 0; i < database_len; i++)
        {
            // printf("%s %s %d\n", database[i].domain, buffer, strcmp(database[i].domain, temp));
            if (!strcmp(database[i].domain, buffer))
            {
                memset(buffer, '\0', sizeof(buffer));
                strcpy(buffer, database[i].ip);
                buffer[strlen(database[i].ip)] = '\0';
                found = 1;
                break;
            }
        }

        if (!found)
        {
            memset(buffer, '\0', sizeof(buffer));
            strcpy(buffer, "IP NOT FOUND\0");
        }

        buffer[strlen(buffer)] = '\0';
        int buff_size = strlen(buffer) * sizeof(char);
        sentBytes = send(newsockfd, buffer, buff_size, 0);
        if (sentBytes == -1)
        {
            printf("\nSending failed. Try again\n");
        }
    }

    close(sockfd);
    close(newsockfd);
}
