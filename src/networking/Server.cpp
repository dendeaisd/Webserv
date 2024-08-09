
#include "../../include/networking/Server.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

using namespace net;

Server::Server(int port) : serverSocket_(AF_INET, SOCK_STREAM, 0) {
  serverSocket_.setSocketOption(SOL_SOCKET, SO_REUSEADDR, 1);
  serverSocket_.bindSocket(port);
  serverSocket_.listenSocket(3);
  serverSocket_.setNonBlocking();
  pollManager_.addSocket(serverSocket_.getSocketFd());
}

Server::~Server() {
  for (std::vector<Client*>::iterator it = clients_.begin();
       it != clients_.end(); ++it) {
    delete *it;
  }
}

void Server::run() {
  while (true) {
    pollManager_.pollSockets();
    handleEvents();
  }
}

void Server::handleEvents() {
  std::vector<struct pollfd>& fds = pollManager_.getFds();

  for (size_t i = 0; i < fds.size(); ++i) {
    if (fds[i].revents & POLLIN) {
      if (fds[i].fd == serverSocket_.getSocketFd()) {
        handleNewConnection();
      } else {
        handleClientRequest(fds[i].fd);
      }
    }
  }
}

void Server::handleClientRequest(int fd) {
  for (std::vector<Client*>::iterator it = clients_.begin();
       it != clients_.end(); ++it) {
    if ((*it)->getFd() == fd) {
      processClientRequest(it);
      break;
    }
  }
}

void Server::processClientRequest(std::vector<Client*>::iterator& it) {
  try {
    if (!(*it)->handleRequest()) {
      cleanupClient(it);
    }
  } catch (const std::exception& e) {
    std::cerr << "Exception caught while handling request: " << e.what()
              << std::endl;
    cleanupClient(it);
  }
}

void Server::cleanupClient(std::vector<Client*>::iterator& it) {
  pollManager_.removeSocket((*it)->getFd());
  delete *it;
  clients_.erase(it);
}

void Server::handleNewConnection() {
  struct sockaddr_in address;
  socklen_t addrlen = sizeof(address);
  int new_socket = serverSocket_.acceptConnection(&address, &addrlen);

  if (new_socket >= 0) {
    Client* new_client = new Client(new_socket);
    clients_.push_back(new_client);
    pollManager_.addSocket(new_socket);
  }
}