#pragma once

#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

/**
 * @brief Main handler for an individual client connection.
 * * Performs the following steps:
 * 
 * reads request
 * validates method
 * resolves file path
 * sends conent or sends error
 * * @param client_fd The socket file descriptor for the connected client.
 */
void handle_client(int client_fd);
