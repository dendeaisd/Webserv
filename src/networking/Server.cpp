
#include "../../include/networking/Server.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>

#include "../../include/Event.hpp"
#include "../../include/log/Log.hpp"

Server::Server(std::vector<int>& ports) {
  for (int port : ports) {
    auto newSocket = std::make_shared<Socket>(AF_INET, SOCK_STREAM, 0);
    newSocket->setSocketOption(SOL_SOCKET, SO_REUSEADDR, 1);
    newSocket->bindSocket(port);
    newSocket->listenSocket(SOMAXCONN);
    newSocket->setNonBlocking();
    _pollManager.addSocket(newSocket->getSocketFd());
    _serverSockets.push_back(newSocket);
  }
}

Server::~Server() {
  for (std::vector<Client*>::iterator it = _clients.begin();
       it != _clients.end(); ++it) {
    delete *it;
  }
}

void Server::run() {
  while (true) {
    _pollManager.pollSockets();
    handleEvents();
  }
}

void Server::handleEvents() {
  std::vector<struct pollfd>& fds = _pollManager.getFds();

  for (size_t i = 0; i < fds.size(); ++i) {
    if (fds[i].revents & POLLIN) {
      auto it = std::find_if(_serverSockets.begin(), _serverSockets.end(),
                             [fd = fds[i].fd](std::shared_ptr<Socket> socket) {
                               return socket->getSocketFd() == fd;
                             });
      if (it != _serverSockets.end()) {
        handleNewConnection((*it)->getSocketFd());
      } else {
        handleClientRequest(fds[i].fd);
      }
    }
  }
  removeCGIEvents();
}

void Server::removeCGIEvents() {
  std::vector<int> eventsToRemove;
  for (auto it = Event::getInstance().getEvents().begin();
       it != Event::getInstance().getEvents().end(); ++it) {
    if (it->second->wait()) {
      Log::getInstance().info("Removing CGI event");
      eventsToRemove.push_back(it->first);
    }
  }
  for (int eventId : eventsToRemove) {
    Event::getInstance().removeEvent(eventId);
  }
}

void Server::handleClientRequest(int fd) {
  for (std::vector<Client*>::iterator it = _clients.begin();
       it != _clients.end(); ++it) {
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
  _pollManager.removeSocket((*it)->getFd());
  delete *it;
  _clients.erase(it);
}

void Server::handleNewConnection(int serverFd) {
  struct sockaddr_in address;
  socklen_t addrlen = sizeof(address);
  auto it = std::find_if(_serverSockets.begin(), _serverSockets.end(),
                         [serverFd](std::shared_ptr<Socket> socket) {
                           return socket->getSocketFd() == serverFd;
                         });

  if (it == _serverSockets.end()) {
    std::cerr << "Error: Server socket not found for fd " << serverFd
              << std::endl;
    return;
  }
  int new_socket = (*it)->acceptConnection(&address, &addrlen);
  if (new_socket >= 0) {
    Client* new_client = new Client(new_socket);
    _clients.push_back(new_client);
    _pollManager.addSocket(new_socket);
  }
}
