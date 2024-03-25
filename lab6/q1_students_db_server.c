// Q1 Students database server using TCP protocol
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
#define PORT_LISTENING 2345
#define LISTEN_QUEUE_LEN 100

#define FAIL_SLEEP_SECONDS 1
#define FAIL_MAX 100

int sockfd, newsockfd;

struct course
{
    char name[100], code[5], mark[4];
};

struct student
{
    char reg[10];
    int semester, courses_count;
    char name[100], address[200], dept[100], section;
    struct course courses[10];
};

struct student database[] = {{
                                 "220911123",
                                 3,
                                 3,
                                 "Raghav",
                                 "Sector 12, Noida",
                                 "ICT",
                                 'B',
                                 {{"Maths", "2121", "65"}, {"Database Systems", "2122", "74"}, {"Computer Networks", "2123", "60"}},
                             },
                             {
                                 "220911145",
                                 4,
                                 2,
                                 "Vishnu",
                                 "Sector 34, Delhi",
                                 "ICT",
                                 'C',
                                 {{"Humanities", "1034", "89"}, {"Embedded Systems", "2126", "94"}},
                             }};
int database_len = sizeof(database) / sizeof(database[0]);

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
    int failCount, retval, receivedBytes, sentBytes, choice;
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

        recv(newsockfd, &choice, sizeof(choice), 0);
        printf("\nChoice: %d", choice);

        pid = fork();

        if (pid == 0)
            continue;

        memset(buffer, '\0', sizeof(buffer));

        receivedBytes = recv(newsockfd, buffer, sizeof(buffer), 0);
        if (receivedBytes == -1 || !buffer[0])
        {
            printf("\nReceive error\n");
            sleep(2);
            continue;
        }
        printf("\nReceived: %s", buffer);

        strncpy(temp, buffer, strlen(buffer));
        temp[strlen(buffer) - 1] = '\0';

        switch (choice)
        {
        case 1:
            snprintf(buffer, MAXSIZE, "PID: %d\nStudent not found\n", pid);
            for (int i = 0; i < database_len; i++)
            {
                // printf("%s %s %d\n", database[i].reg, temp, strcmp(database[i].reg, temp));
                if (!strcmp(database[i].reg, temp))
                {
                    snprintf(buffer, MAXSIZE, "PID: %d\nName: %s\nAddress: %s\n",
                             pid,
                             database[i].name,
                             database[i].address);
                    break;
                }
            }
            break;
        case 2:
            snprintf(buffer, MAXSIZE, "PID: %d\nStudent not found\n", pid);
            for (int i = 0; i < database_len; i++)
            {
                if (!strcmp(database[i].name, temp))
                {
                    snprintf(buffer, MAXSIZE, "PID: %d\nDepartment: %s\nSemester: %d\nSection: %c\nCourses: ",
                             pid,
                             database[i].dept,
                             database[i].semester,
                             database[i].section);
                    for (int j = 0; j < database[i].courses_count; j++)
                    {
                        strcat(buffer, database[i].courses[j].name);
                        if (j != database[i].courses_count - 1)
                            strcat(buffer, ", ");
                    }
                    break;
                }
            }
            break;
        case 3:
            snprintf(buffer, MAXSIZE, "PID: %d\nGiven course code not found", pid);
            for (int i = 0; i < database_len; i++)
            {
                for (int j = 0; j < database[i].courses_count; j++)
                {
                    // printf("%d %d %s %s %d\n", i, j, database[i].courses[j].code, temp, strcmp(database[i].courses[i].code, temp));
                    if (!strcmp(database[i].courses[j].code, temp))
                    {
                        snprintf(buffer, MAXSIZE, "PID: %d\nMarks for course code %s: %s", pid, temp, database[i].courses[j].mark);
                        break;
                    }
                }
            }
            break;
        default:
            printf("\ninvalid choice");
            continue;
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
