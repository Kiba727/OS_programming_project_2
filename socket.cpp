#include "socket.h"

#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <iostream>

int create_listen_socket(int port, int backlog) {
    int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        std::perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);

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