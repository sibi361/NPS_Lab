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

#define LEAK_RATE 1
#define BUCKET_SIZE 10
#define PACKET_SIZE 4

// Structure representing the leaky bucket
struct LeakyBucket
{
    int current_size;
};

// Initialize the leaky bucket
void initializeBucket(struct LeakyBucket *bucket)
{
    bucket->current_size = 0;
}

// Add packet to the leaky bucket
int addPacket(struct LeakyBucket *bucket, int packet_size)
{
    if ((bucket->current_size + packet_size) <= BUCKET_SIZE)
    {
        bucket->current_size += packet_size;
        return 1; // Packet added successfully
    }
    else
    {
        return 0; // Packet dropped due to overflow
    }
}

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
            listeningPort += 1;
        }
    }

    int current_time = 0, arrival_time;

    struct LeakyBucket bucket;
    initializeBucket(&bucket);

    recv(newsockfd, &arrival_time, sizeof(int), 0);

    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);
        if (newsockfd == -1)
        {
            continue;
        }
        // printf("\nReceived connection from client on fd: %d\n", newsockfd);

        recv(newsockfd, &arrival_time, sizeof(int), 0);
        printf("Received packet at time: %d\n", arrival_time);

        // Simulate time passing
        while (current_time < arrival_time)
        {
            // Leak the bucket by one byte per second
            bucket.current_size -= LEAK_RATE;
            if (bucket.current_size < 0)
            {
                bucket.current_size = 0;
            }
            current_time++;

            // Display the current time and bucket status
            // printf("%d\t-\tBucket Size: %d\n", current_time, bucket.current_size);
        }

        // Add the current packet to the bucket
        int conforming = addPacket(&bucket, PACKET_SIZE);

        memset(buffer, '\0', sizeof(buffer));
        if (conforming)
        {
            strcpy(buffer, "Conforming\n");
        }
        else
        {
            strcpy(buffer, "Nonconforming\n");
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
