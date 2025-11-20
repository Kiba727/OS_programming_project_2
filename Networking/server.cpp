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

    //create copy of client socket
    int clientSocket = accept(serverSocket, nullptr, nullptr);

    //create buffer to recieve data from client
    char buffer[1024] = {0};
    recv(clientSocket, buffer, sizeof(buffer), 0);
    cout << "Message from client: " << buffer
              << endl;

    close(serverSocket);

    return 0;
}