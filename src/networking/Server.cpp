
#include "../../include/networking/Server.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "../../include/Event.hpp"
#include "../../include/log/Log.hpp"

Server::Server(const std::vector<int>& ports) {
  for (int port : ports) {
    Socket newSocket(AF_INET, SOCK_STREAM, 0);
    newSocket.setSocketOption(SOL_SOCKET, SO_REUSEADDR, 1);
    newSocket.bindSocket(port);
    newSocket.listenSocket(SOMAXCONN);
    newSocket.setNonBlocking();
    pollManager_.addSocket(newSocket.getSocketFd());
    serverSockets_.push_back(std::move(newSocket));
  }
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
      // Check if the fd is one of the server sockets
      auto it = std::find_if(serverSockets_.begin(), serverSockets_.end(),
                             [fd = fds[i].fd](const Socket& socket) {
                               return socket.getSocketFd() == fd;
                             });
      if (it != serverSockets_.end()) {
        // If it matches, handle new connection on that specific server socket
        handleNewConnection(it->getSocketFd());
      } else {
        // Otherwise, handle client request
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

void Server::handleNewConnection(int serverFd) {
  struct sockaddr_in address;
  socklen_t addrlen = sizeof(address);
  auto it = std::find_if(serverSockets_.begin(), serverSockets_.end(),
                         [serverFd](const Socket& socket) {
                           return socket.getSocketFd() == serverFd;
                         });

  if (it == serverSockets_.end()) {
    std::cerr << "Error: Server socket not found for fd " << serverFd
              << std::endl;
    return;
  }
  int new_socket = it->acceptConnection(&address, &addrlen);
  if (new_socket >= 0) {
    Client* new_client = new Client(new_socket);
    clients_.push_back(new_client);
    pollManager_.addSocket(new_socket);
  }
}
