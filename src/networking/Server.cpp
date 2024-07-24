
#include "../../include/networking/Server.hpp"
#include <iostream>
#include <ostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

using namespace net;

Server::Server(int port, const std::string& address) : serverSocket_(AF_INET, SOCK_STREAM, 0) {
    serverSocket_.Bind(port, address);
    serverSocket_.Listen(128);
    pollManager_.addSocket(serverSocket_.getFd());
}

void Server::run() {
    std::cout << "Server is listening on port 8080..." << std::endl;

    while (true) {
        int pollCount = pollManager_.pollSockets(250000); // Use a reasonable timeout value
        if (pollCount == -1) {
            perror("poll error");
            break;
        }

        for (size_t i = 0; i < pollManager_.getPollSize(); ++i) {
            struct pollfd& poll = pollManager_.getPollFd(i);

            if (poll.revents & POLLIN) {
                if (poll.fd == serverSocket_.getFd()) {
                    handleNewConnection();
                } else {
                    std::cout << "am i coming here?\n" << std::endl;                     handleClientData(poll.fd);
                }
            } else if (poll.revents != 0) { // Handle other poll events (errors, hangups, etc.)
                handlePollError(poll.fd, "Poll error");
            }
        }
    }
}

void Server::handleNewConnection() {
    try {
        net::Socket clientSocket = serverSocket_.Accept();

        // Set the new socket to non-blocking mode
        int flags = fcntl(clientSocket.getFd(), F_GETFL, 0);
        if (flags == -1) {
            throw std::runtime_error("fcntl F_GETFL failed");
        }
        if (fcntl(clientSocket.getFd(), F_SETFL, flags | O_NONBLOCK) == -1) {
            throw std::runtime_error("fcntl F_SETFL failed");
        }

        pollManager_.addSocket(clientSocket.getFd());
        clientConnections_[clientSocket.getFd()] = true; // Mark the client as connected
        std::cout << "Accepted a new connection with fd: " << clientSocket.getFd() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error accepting new connection: " << e.what() << std::endl;
    }
}

void Server::handleClientData(int clientFd) {
    char buffer[1024];
    int bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead <= 0) {
        if (bytesRead == 0) {
            std::cout << "Client disconnected, fd: " << clientFd << std::endl;
        } else if (bytesRead < 0 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
            perror("recv error");
        }
        clientConnections_[clientFd] = false; // Mark the client as disconnected
    } else {
        buffer[bytesRead] = '\0';
        std::cout << "Received data from fd " << clientFd << ": " << buffer << std::endl;

        // Simple HTTP response
        std::string httpResponse =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 13\r\n"
                "\r\n"
                "Hello, World!";
        ssize_t sentBytes = send(clientFd, httpResponse.c_str(), httpResponse.size(), 0);

        if (sentBytes == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("send error");
                clientConnections_[clientFd] = false; // Mark as disconnected on send error
            } else {
                // Handle the case where the send buffer is full
            }
        } else {
            std::cout << "Sent HTTP response" << std::endl;
        }
    }

    // Close the connection if it's marked as disconnected
    if (!clientConnections_[clientFd]) {
        pollManager_.removeSocket(clientFd);
        close(clientFd);
        clientConnections_.erase(clientFd);
    }
}

void Server::handlePollError(int fd, const std::string& errorMessage) {
  std::cerr << errorMessage << " on fd: " << fd << " - " << strerror(errno) << std::endl;
  pollManager_.removeSocket(fd);
  close(fd);
}

