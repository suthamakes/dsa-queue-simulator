#ifndef SERVER_H
#define SERVER_H

#define PORT 8080

extern struct sockaddr_in *address;

int create_socket();
void set_socket_options(int sock_fd);
void bind_socket(int sock_fd, struct sockaddr_in *address);
void listen_for_connections(int sock_fd);
int accept_connection(int sock_fd, struct sockaddr_in *address);

#endif