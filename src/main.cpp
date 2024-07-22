#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "../include/networking/Socket.hpp"
#include "../include/networking/SocketExceptions.hpp"


int main() {
    try {
        net::Socket serverSocket(AF_INET, SOCK_STREAM, 0);
        serverSocket.Bind(8080, "127.0.0.1");
        serverSocket.Listen(10);

        std::cout << "Server is listening on port 8080..." << std::endl;

        while (true) {
            net::Socket clientSocket = serverSocket.Accept();
            std::cout << "Accepted a new connection..." << std::endl;

            // Test if the client socket is in non-blocking mode
            char buffer[1];
            int result = recv(clientSocket.getFd(), buffer, sizeof(buffer), 0);
            if (result == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                std::cout << "Client socket is in non-blocking mode." << std::endl;
            } else {
                std::cerr << "Client socket is not in non-blocking mode or another error occurred." << std::endl;
            }

            // Simple HTTP response
            std::string httpResponse = 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 13\r\n"
                "\r\n"
                "Hello, World!";

            // Send HTTP response to the client
            send(clientSocket.getFd(), httpResponse.c_str(), httpResponse.size(), 0);

            // Close the client socket
            clientSocket.~Socket();
        }

    } catch (const net::socketException &e) {
        std::cerr << "Socket exception: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

