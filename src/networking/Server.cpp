
#include "../../include/networking/Server.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

using namespace net;

Server::Server(int port) : port_(port), serverSocket_(AF_INET, SOCK_STREAM, 0) {
  if (!serverSocket_.Bind(port, "127.0.0.1")) {
    throw std::runtime_error("Error binding server socket: " +
                             std::string(strerror(errno)));
  }
  if (!serverSocket_.Listen(10)) {
    throw std::runtime_error("Error listening on server socket: " +
                             std::string(strerror(errno)));
  }
  pollManager_.addSocket(serverSocket_.getFd(), POLLIN);
}

void Server::run() {
  while (true) {
    int numEvents = pollManager_.pollSockets(1000);
    if (numEvents > 0) {
      handleEvents(numEvents);
    }
  }
}

void Server::handleEvents(int numEvents) {
  for (size_t i = 0; i < pollManager_.getPollSize(); ++i) {
    struct pollfd pfd = pollManager_.getPollFd(i);
    if (pfd.revents & POLLERR || pfd.revents & POLLHUP ||
        pfd.revents & POLLNVAL) {
      handleSocketError(pfd.fd);
    } else if (pfd.fd == serverSocket_.getFd() && (pfd.revents & POLLIN)) {
      acceptNewClient();
    } else if (pfd.revents & POLLIN) {
      handleClientData(pfd.fd);
    }
  }
}

void Server::acceptNewClient() {
  Socket newClient = serverSocket_.Accept();
  if (newClient.getFd() != -1) {
    pollManager_.addSocket(newClient.getFd(), POLLIN);
    clients_.insert(
        std::make_pair(newClient.getFd(), Client(newClient.getFd())));
    std::cout << "New client connected: " << newClient.getFd() << std::endl;
  } else {
    throw std::runtime_error("Error accepting new client: " +
                             std::string(strerror(errno)));
  }
}

void Server::handleClientData(int clientFd) {
  std::map<int, Client>::iterator it = clients_.find(clientFd);
  if (it != clients_.end()) {
    if (it->second.getFd() == clientFd &&
        isValidSocket(clientFd)) { // Add a check for socket validity
      if (!it->second.handleData()) {
        std::cout << "Client disconnected: " << clientFd << std::endl;
        clients_.erase(it);
        pollManager_.removeSocket(clientFd);
        close(clientFd); // Close the client socket
      }
    } else {
      // Handle invalid socket error
      std::cerr << "Invalid socket: " << clientFd << std::endl;
      clients_.erase(it);
      pollManager_.removeSocket(clientFd);
      close(clientFd); // Close the client socket
    }
  }
}

bool Server::isValidSocket(int fd) {
  return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

void Server::handleSocketError(int fd) {
  if (isValidSocket(fd)) {
    std::cout << "Socket error: " << fd << std::endl;
    clients_.erase(fd);
    pollManager_.removeSocket(fd);
    close(fd); // Close the socket
  } else {
    // Handle invalid socket error
    std::cerr << "Invalid socket: " << fd << std::endl;
  }
}
