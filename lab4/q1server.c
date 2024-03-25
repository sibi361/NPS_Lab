#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 8888
#define BUFFER_SIZE 256

int checkParity(const char *data) {
    int parity = 0;
    for (int i = 0; i < strlen(data) - 1; i++) {
        if (data[i] == '1') {
            parity ^= 1;
        }
    }
    return parity == (data[strlen(data) - 1] - '0');
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for a connection...\n");

    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);

    if (client_socket < 0) {
        perror("Accept failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Receive data from the client
    recv(client_socket, buffer, BUFFER_SIZE, 0);

    // Check parity
    if (checkParity(buffer)) {
        printf("Received data with correct parity: %s\n", buffer);
    } else {
        printf("Received data with incorrect parity: %s\n", buffer);
    }

    close(server_socket);
    close(client_socket);
    return 0;
}

