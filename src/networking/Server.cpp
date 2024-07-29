
#include "../../include/networking/Server.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <cstring>

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
    cleanupClients();
  }
}


void Server::handleEvents() {
    std::vector<struct pollfd>& fds = pollManager_.getFds();

    for (size_t i = 0; i < fds.size(); ++i) {
        if (fds[i].revents & POLLIN) {
            if (fds[i].fd == serverSocket_.getSocketFd()) {
                handleNewConnection();
            } else {
                for (std::vector<Client*>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
                    if ((*it)->getFd() == fds[i].fd) {
                        if (!(*it)->handleRequest()) {
                            pollManager_.removeSocket((*it)->getFd());
                            delete *it;
                            clients_.erase(it);
                            break;
                        }
                    }
                }
            }
        }
    }
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

void Server::handleClientData(Client* client) {
  client->handleRequest();

  if (client->getFd() == -1) {
    pollManager_.removeSocket(client->getFd());
  }
}

void Server::cleanupClients() {
  std::vector<Client*>::iterator it = clients_.begin();
  while (it != clients_.end()) {
    if ((*it)->getFd() == -1) {
      delete *it;
      it = clients_.erase(it);
    } else {
     ++it;
    }
  }
}
