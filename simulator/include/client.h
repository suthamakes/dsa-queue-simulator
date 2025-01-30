#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int create_socket();
void connect_to_server(int sock, const char *ip_address);
void receive_data(int sock);

#endif