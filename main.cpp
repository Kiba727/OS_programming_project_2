#include "socket.h"
#include "http_parser.h"

#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

int main() { 
    int port = 8080;
    // This is the Server Socket 
    // Asks the OS for a TCP (Transmission Control Protocol) socket boudn to port 8080
    // Puts it into "Listening Mode"
    int listen_fd = create_listen_socket(port);

    if (listen_fd < 0) {
        std::cerr << "Failed to open socket\n";
        return 1;
    }

    // This is the "Accept" Loop
    while (true) {
        sockaddr_in client{};
        int client_fd = accept_client(listen_fd, client);

        if (client_fd < 0) continue;

        std::cout << "[+] Client connected\n";
        handle_client(client_fd);
        close(client_fd);
        std::cout << "[-] Client disconnected\n";
    }

    close(listen_fd);
    return 0;
}