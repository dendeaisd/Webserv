
#include "../../include/networking/Server.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <ostream>

using namespace net;

Server::Server(int port) : port_(port), serverSocket_(AF_INET, SOCK_STREAM, 0) {
  if (!serverSocket_.Bind(port, "0.0.0.0")) {
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
      for (size_t i = 0; i < pollManager_.getPollSize(); ++i) {
        pollfd& pfd = pollManager_.getPollFd(i);
        if (pfd.fd == serverSocket_.getFd() && (pfd.revents & POLLIN)) {
          acceptNewClient();
        } else if (pfd.revents & POLLIN) {
          handleClientData(pfd.fd);
        }
      }
    }
  }
}

void Server::acceptNewClient() {
  Socket newClient = serverSocket_.Accept();
  if (newClient.getFd() != -1) {
    pollManager_.addSocket(newClient.getFd(), POLLIN);
    clients_.emplace(newClient.getFd(), newClient.getFd());
    std::cout << "New client connected: " << newClient.getFd() << std::endl;
  } else {
    std::cerr << "Error accepting new client: " << strerror(errno) << std::endl;
  }
}

void Server::handleClientData(int clientFd) {
  auto it = clients_.find(clientFd);
  if (it != clients_.end()) {
    it->second.handleData();
    if (it->second.getFd() == -1) {
      std::cout << "Client disconnected: " << clientFd << std::endl;
      clients_.erase(it);
      pollManager_.removeSocket(clientFd);
    }
  }
}