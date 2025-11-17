#pragma once
#include <netinet/in.h>

int create_listen_socket(int port, int backlog = 10);
int accept_client(int listen_fd, sockaddr_in &client_addr);
