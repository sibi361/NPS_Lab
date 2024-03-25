#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define BUFFER_SIZE 256

int calculateParity(const char *data) {
    int parity = 0;
    for (int i = 0; i < strlen(data); i++) {
        if (data[i] == '1') {
            parity ^= 1;
        }
    }
    return parity;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Enter binary data to transmit: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline character

    // Calculate even parity and append it to the data
    int parity = calculateParity(buffer);
    sprintf(buffer + strlen(buffer), "%d", parity);

    // Send data to the server
    send(client_socket, buffer, strlen(buffer), 0);

    close(client_socket);
    return 0;
}

