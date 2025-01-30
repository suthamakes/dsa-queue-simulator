#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

struct sockaddr_in *address;

int create_socket()
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    return sock_fd;
}

void set_socket_options(int sock_fd) {
    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
}

void bind_socket(int sock_fd, struct sockaddr_in *address) {
    if (bind(sock_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
}

void listen_for_connections(int sock_fd) {
    if (listen(sock_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
}

int accept_connection(int sock_fd, struct sockaddr_in *address) {
    int addrlen = sizeof(*address);
    int new_socket = accept(sock_fd, (struct sockaddr *)address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    return new_socket;
}