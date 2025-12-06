#include "socket.h"
#include "http_parser.h"

#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t thread_limiter;

void* client_worker(void* arg) {
    int client_fd = (int)(intptr_t)arg;

    handle_client(client_fd);
    
    close(client_fd);
    printf("[-] Client disconnected (FD: %d)\n", client_fd);
    sem_post(&thread_limiter);
    return NULL;
}

int main() { 
    int port = 8080;
    // This is the Server Socket 
    // Asks the OS for a TCP (Transmission Control Protocol) socket boudn to port 8080
    // Puts it into "Listening Mode"
    int listen_fd = create_listen_socket(port);

    sem_init(&thread_limiter, 0, 5);

    printf("Server listening on port %d...\n", port);

    // This is the "Accept" Loop
    while (1) {
            sem_wait(&thread_limiter);

            sockaddr_in client;
            int client_fd = accept_client(listen_fd, client);

            if (client_fd < 0) {
                sem_post(&thread_limiter);
                continue;
            }

            pthread_t thread_id;
            
            if (pthread_create(&thread_id, NULL, client_worker, (void*)(intptr_t)client_fd) != 0) {
                perror("Thread creation failed");
                close(client_fd);
                sem_post(&thread_limiter);
            } else {
                pthread_detach(thread_id);
            }
        }

        sem_destroy(&thread_limiter);
        close(listen_fd);
        return 0;
    }