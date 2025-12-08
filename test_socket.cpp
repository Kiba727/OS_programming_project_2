/**
 * @file test_socket.cpp
 * @brief Test driver for socket.cpp
 * Compile with: g++ -o test_socket test_socket.cpp socket.cpp
 */

#include "socket.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <cstring>

int main() {
    std::cout << "=== Starting Socket Test ===\n";

    int port = 9090; // Use a different port than main server to avoid conflict
    
    // 1. Test Creation
    int listen_fd = create_listen_socket(port);
    if (listen_fd < 0) {
        std::cerr << "[FAIL] Could not create listen socket.\n";
        return 1;
    }
    std::cout << "[PASS] Listen socket created on port " << port << " (FD: " << listen_fd << ")\n";

    // Fork a child process to simulate a client
    pid_t pid = fork();

    if (pid == 0) {
        // --- CHILD PROCESS (Client) ---
        sleep(1); // Give parent time to hit accept()
        
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("[Child] Connect failed");
            exit(1);
        }
        std::cout << "[Child] Connected to server successfully.\n";
        close(sock);
        exit(0);
    } 
    else {
        // --- PARENT PROCESS (Server) ---
        std::cout << "[Parent] Waiting for client connection...\n";
        
        sockaddr_in client_addr;
        int client_fd = accept_client(listen_fd, client_addr);

        if (client_fd >= 0) {
            std::cout << "[PASS] Accepted client connection (FD: " << client_fd << ")\n";
            close(client_fd);
        } else {
            std::cerr << "[FAIL] Accept failed.\n";
        }

        // Wait for child to exit
        wait(NULL); 
        close(listen_fd);
    }

    return 0;
}