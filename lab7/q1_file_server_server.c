// Q1 File Server
// SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

#define MAXSIZE 100
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

void str_replace(char *target, const char *find, const char *replaceWith)
{
    char buffer[1024] = {0};
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(find);
    size_t repl_len = strlen(replaceWith);

    while (1)
    {
        const char *p = strstr(tmp, find);

        if (p == NULL)
        {
            strcpy(insert_point, tmp);
            break;
        }

        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        memcpy(insert_point, replaceWith, repl_len);
        insert_point += repl_len;

        tmp = p + needle_len;
    }

    // write altered string back to target
    strcpy(target, buffer);
}

void main()
{
    signal(SIGINT, exitServer);

    socklen_t actuallen;
    int failCount, retval, receivedBytes, sentBytes, buff_size, choice;
    int listeningPort, listeningPortGiven, len;
    struct sockaddr_in serveraddr, clientaddr;
    char buffer[MAXSIZE], temp[MAXSIZE], filename[MAXSIZE], tmp1[MAXSIZE], tmp2[MAXSIZE], swap_tmp;
    FILE *file;

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
        printf("\nReceived: %s", buffer);

        buffer[strlen(buffer) - 1] = '\0';

        // file doesn't exist
        if (!access(buffer, F_OK) == 0)
        {
            strcpy(buffer, "File not present\n");
        }
        else
        {
            memset(filename, '\0', sizeof(buffer));
            strncpy(filename, buffer, sizeof(buffer));
            strcpy(buffer, "1. Search\n2. Replace\n3. Sort\n4. Exit\nEnter choice: ");
        }

        buffer[strlen(buffer)] = '\0';
        buff_size = strlen(buffer) * sizeof(char);
        sentBytes = send(newsockfd, buffer, buff_size, 0);
        if (sentBytes == -1)
        {
            printf("\nSending failed. Try again\n");
        }

        /////////

        recv(newsockfd, &choice, sizeof(choice), 0);
        printf("\nChoice: %d", choice);

        file = fopen(filename, "r");
        while (fgets(temp, sizeof(temp), file) != NULL)
            ;
        fclose(file);
        temp[strlen(temp)] = '\0';
        printf("\nfile contents: %s\n", temp);

        switch (choice)
        {
        case 1:
            memset(buffer, '\0', sizeof(buffer));
            recv(newsockfd, buffer, sizeof(buffer), 0);

            memset(buffer, '\0', sizeof(buffer));
            if (strstr(temp, buffer) != NULL)
            {
                strcat(buffer, "String is present");
            }
            else
            {
                strcat(buffer, "String is NOT present");
            }
            break;
        case 2:
            memset(tmp1, '\0', sizeof(tmp1));
            recv(newsockfd, tmp1, sizeof(tmp1), 0);
            temp[strlen(tmp1) - 1] = '\0';
            printf("\nbuf: %s", tmp1);

            memset(tmp2, '\0', sizeof(tmp2));
            recv(newsockfd, tmp2, sizeof(tmp2), 0);
            temp[strlen(tmp2) - 1] = '\0';
            printf("\nbuf: %s", tmp2);

            str_replace(temp, tmp1, tmp2);
            printf("\nbuf: %s", temp);
            strcpy(buffer, temp);

            file = fopen(filename, "w");
            if (file == NULL)
            {
                printf("Error opening file!\n");
                exit(1);
            }

            fprintf(file, temp);
            fclose(file);

            break;
        case 3:
            len = strlen(temp);
            for (int i = 0; i < len - 1; i++)
            {
                for (int j = 0; j < len - 1 - i; j++)
                {
                    if (temp[j] > temp[j + 1])
                    {
                        swap_tmp = temp[j];
                        temp[j] = temp[j + 1];
                        temp[j + 1] = swap_tmp;
                    }
                }
            }

            file = fopen(filename, "w");
            if (file == NULL)
            {
                printf("Error opening file!\n");
                exit(1);
            }

            fprintf(file, temp);
            fclose(file);

            memset(buffer, '\0', sizeof(buffer));
            strcat(buffer, "Sorting done");
            break;
        default:
            printf("\nInvalid choice");
            continue;
        }

        buffer[strlen(buffer)] = '\0';
        buff_size = strlen(buffer) * sizeof(char);
        sentBytes = send(newsockfd, buffer, buff_size, 0);
        if (sentBytes == -1)
        {
            printf("\nSending failed. Try again\n");
        }
    }

    close(sockfd);
    close(newsockfd);
}
