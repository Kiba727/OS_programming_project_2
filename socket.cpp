#include "socket.h"

int create_listen_socket(int port, int backlog) {
    // Creates a TCP socket for IPv4 Networking 
    // Creates a socket object inside the kernel 
    // The OS returns a file descriptor (an integer) to refer to that socket
    int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        std::perror("socket");
        return -1;
    }
    // Allows use to reuse Socket address without waiting for timeout
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Stores address and port in a struct
    sockaddr_in addr{};
    addr.sin_family      = AF_INET; // IPv4 Address Family
    addr.sin_addr.s_addr = INADDR_ANY; // Bind to all local interfaces 
    addr.sin_port        = htons(port); // Convert port to network byte order using most        signifcant byte first

    if (bind(listen_fd, (sockaddr *)&addr, sizeof(addr)) < 0) {
        std::perror("bind");
        close(listen_fd);
        return -1;
    }

    if (listen(listen_fd, backlog) < 0) {
        std::perror("listen");
        close(listen_fd);
        return -1;
    }

    std::cout << "[+] Listening on port " << port << std::endl;
    return listen_fd;
}

int accept_client(int listen_fd, sockaddr_in &client_addr) {
    socklen_t len = sizeof(client_addr);
    int client_fd = accept(listen_fd, (sockaddr *)&client_addr, &len);
    if (client_fd < 0) {
        std::perror("accept");
        return -1;
    }
    return client_fd;
}