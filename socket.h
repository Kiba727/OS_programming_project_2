#pragma once
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <iostream>

/**
 * @brief Creates and configures a listening TCP socket.
 * 
 * This function creates a socket in the following steps
 * creates a file descriptor
 * binds socket with port 8080
 * marks socket to be ready to accept connections
 *
 * @param port The port number to listen on 
 * @param backlog The maximum length of the queue connections
 * @return The file descriptor of the listening socket, or -1 on error.
 */
int create_listen_socket(int port, int backlog = 10);

/**
 * @brief Accepts a new incoming client connection.
 *
 * Blocks and waits until a client connects.
 * When a connection is made returns a new file descriptor
 *
 * @param listen_fd The listening socket file descriptor created by create_listen_socket().
 * @param client_addr Reference to a sockaddr_in struct to store the client's address details.
 * @return The file descriptor for the connected client, or -1 on error.
 */
int accept_client(int listen_fd, sockaddr_in &client_addr);