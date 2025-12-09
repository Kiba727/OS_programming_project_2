/**
 * @file main.cpp
 * @brief Multi-threaded HTTP Server Entry Point.
 * @author: John, Zack, Patrick
 * 
 * This file implements the main server loop. It uses a "Thread-per-connection"
 * model, but throttled by a semaphore to prevent resource exhaustion.
 * * Architecture:
 * 1. Initialize a listening socket.
 * 2. Initialize a semaphore (token bucket) with 5 tokens.
 * 3. Loop forever:
 * a. Wait for a token (sem_wait).
 * b. Accept a client.
 * c. Spawn a thread to handle the client.
 * d. Thread releases token (sem_post) when finished.
 */

#include "socket.h"
#include "http_parser.h"
#include "thread_pool.h"

#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

sem_t thread_limiter;

/**
 * @brief Thread entry point for handling a client.
 * * This function is executed by each new thread. It handles the specific
 * logic for one client connection and manages resource cleanup.
 * * @param arg A void pointer containing the client socket file descriptor (cast from int).
 * @return NULL (Standard pthread return).
 */
void client_worker(void* arg) {
    int client_fd = (int)(intptr_t)arg;

    handle_client(client_fd);
    
    close(client_fd);
    printf("[-] Client disconnected (FD: %d)\n", client_fd);
}

int main() { 
    int port = 8080;
    // This is the Server Socket 
    // Asks the OS for a TCP (Transmission Control Protocol) socket boudn to port 8080
    // Puts it into "Listening Mode"
    int listen_fd = create_listen_socket(port);
    printf("Server listening on port %d...\n", port);

    ThreadPool pool;
    int created = pool_init(&pool, 5);
    if (created < 0) {
        fprintf(stderr, "Error allocating memory\n");
        return(-1);
    }

    // Accept look where clients are queued to threadpool
    while (1) {
        sockaddr_in client;
        int client_fd = accept_client(listen_fd, client);

        if (client_fd < 0) {
            continue;
        }

        // Enqueue the client where the workers will handle it
        pool_enqueue(&pool, client_fd);
    }
    
    pool_destroy(&pool);
    close(listen_fd);
    return 0;
}
