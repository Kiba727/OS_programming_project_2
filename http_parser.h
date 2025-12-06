#include "http_parser.h"

#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

namespace {

// helper to check suffix
bool ends_with(const std::string &str, const std::string &suffix) {
    if (suffix.size() > str.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

bool send_all(int fd, const std::string &data) {
    size_t sent = 0;
    size_t len = data.size();
    const char* buf = data.data();

    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, 0);
        if (n <= 0) return false;
        sent += n;
    }
    return true;
}

std::string guess_mime(const std::string &p) {
    if (ends_with(p, ".html")) return "text/html";
    if (ends_with(p, ".css"))  return "text/css";
    if (ends_with(p, ".js"))   return "application/javascript";
    if (ends_with(p, ".txt"))  return "text/plain";
    if (ends_with(p, ".png"))  return "image/png";
    if (ends_with(p, ".jpg") || ends_with(p, ".jpeg")) return "image/jpeg";
    return "application/octet-stream";
}

std::string fs_path(const std::string &uri) {
    if (uri == "/") return "./www/index.html";
    return "./www" + uri;
}

void send_simple(int fd, int code, const std::string &reason, const std::string &body) {
    std::ostringstream oss;
    oss << "HTTP/1.0 " << code << " " << reason << "\r\n";
    oss << "Content-Type: text/html\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n\r\n";
    oss << body;
    send_all(fd, oss.str());
}

} // namespace

void handle_client(int client_fd) { 
    sleep(10);  // 10 seconds
    char buf[4096];
    ssize_t n = recv(client_fd, buf, sizeof(buf)-1, 0);
    if (n <= 0) return;
    buf[n] = '\0';

    std::string req(buf);
    std::istringstream ss(req);
    std::string method, uri, version;

    ss >> method >> uri >> version;

    std::cout << "[REQ] " << method << " " << uri << std::endl;

    if (method != "GET") {
        send_simple(client_fd, 405, "Method Not Allowed", "<h1>405 Not Allowed</h1>");
        return;
    }

    std::string path = fs_path(uri);
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        send_simple(client_fd, 404, "Not Found", "<h1>404 Not Found</h1>");
        return;
    }

    std::string body((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    std::string mime = guess_mime(path);

    std::ostringstream oss;
    oss << "HTTP/1.0 200 OK\r\n";
    oss << "Content-Type: " << mime << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n\r\n";

    send_all(client_fd, oss.str());
    send_all(client_fd, body); 

    
}