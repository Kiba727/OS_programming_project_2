#pragma once
#include <netinet/in.h>

/**
 * @brief Creates and configures a listening TCP socket.
 *
 * This function performs the standard sequence of socket setup:
 * 1. socket(): Creates the file descriptor.
 * 2. setsockopt(): Sets SO_REUSEADDR to allow immediate restart of the server.
 * 3. bind(): Associates the socket with the specified port on all interfaces.
 * 4. listen(): Marks the socket as passive, ready to accept connections.
 *
 * @param port The port number to listen on (e.g., 8080).
 * @param backlog The maximum length of the queue of pending connections.
 * @return The file descriptor of the listening socket, or -1 on error.
 */
int create_listen_socket(int port, int backlog = 10);

/**
 * @brief Accepts a new incoming client connection.
 *
 * This function blocks (waits) until a client connects to the server.
 * When a connection is established, it returns a new file descriptor
 * specific to that client session.
 *
 * @param listen_fd The listening socket file descriptor created by create_listen_socket().
 * @param client_addr Reference to a sockaddr_in struct to store the client's address details.
 * @return The file descriptor for the connected client, or -1 on error.
 */
int accept_client(int listen_fd, sockaddr_in &client_addr);