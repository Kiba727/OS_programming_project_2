#pragma once

/**
 * @brief Main handler for an individual client connection.
 * * Performs the following steps:
 * 1. Simulates latency (sleep).
 * 2. Reads the request.
 * 3. Validates the method (GET only).
 * 4. Resolves the file path.
 * 5. Sends the file content or an error response.
 * * @param client_fd The socket file descriptor for the connected client.
 */
void handle_client(int client_fd);
