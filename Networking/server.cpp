#include "server.hpp"

using namespace std;

int main()
{
    //Create server socket
    //AF_INET is IPv4 protocol
    //SOCK_STREAM is a TCP socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    //htons converts port into bytes
    //INADDR_ANY accepts connections to any IP (might change?)
    //sockaddr_in is a data type to store socket address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    //bind socket to address
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    //make socket wait and listen for incoming connections
    listen(serverSocket, 5);

    cout << "Server running on http://localhost:8080" << endl; 

    ifstream file("../temp.html"); 
    if (!file.is_open()) {
        cerr << "Failed to open temp.html" << endl;
        return 1;
    }
    stringstream buffer; 
    buffer << file.rdbuf(); 
    string html = buffer.str();

    while (true){
        //create copy of client socket
        int clientSocket = accept(serverSocket, nullptr, nullptr);

        //create buffer to recieve data from client
        char buffer[1024] = {0};
        recv(clientSocket, buffer, sizeof(buffer), 0);

        cout << "Message from client: " << buffer
                << endl;
        
        string http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + to_string(html.size()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            html;

        send(clientSocket, http_response.c_str(), http_response.size(), 0);
        close(clientSocket);
    }

        close(serverSocket);

    return 0;
}