
#include "../../include/networking/Server.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>

#include "../../include/Event.hpp"
#include "../../include/log/Log.hpp"
#include "ConfigFile.hpp"
#include "ServerContext.hpp"

Server::Server(std::vector<int>& ports, std::unique_ptr<ConfigFile>&& config) {
  _config = std::move(config);
  buildPortToServer();
  _config->printConfigFile();
  for (int port : ports) {
    auto newSocket = std::make_shared<Socket>(AF_INET, SOCK_STREAM, 0);
    newSocket->setSocketOption(SOL_SOCKET, SO_REUSEADDR, 1);
    newSocket->bindSocket(port);
    newSocket->listenSocket(SOMAXCONN);
    newSocket->setNonBlocking();
    _pollManager.addSocket(newSocket->getSocketFd(), port);
    _serverSockets.push_back(newSocket);
    Log::getInstance().debug("Server listening on port " +
                             std::to_string(port));
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
    int fd = fds[i].fd;
    short revents = fds[i].revents;
    if (Event::getInstance().hasEvent(fd)) {
      if (Event::getInstance().getEvent(fd)->wait()) {
        auto client = _fdToClientMap.find(fd);
        if (client != _fdToClientMap.end()) {
          client->second->setReadyForResponse(true);
          fds[i].events = POLLOUT;
        }
      }
    }
	Log::getInstance().debug("fd: " + std::to_string(fd) + " revents: " + std::to_string(revents));
    if (revents & POLLIN) {
      Log::getInstance().debug("POLLIN event on fd " + std::to_string(fd));
      handlePollInEvent(fd, fds[i].events);
    } else if (revents & POLLOUT) {
      handlePollOutEvent(fd, fds[i].events);
    } else if (revents & POLLHUP) {
      handlePollHupEvent(fd);
    } else if (revents & POLLERR) {
      handlePollErrEvent(fd);
    }
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

bool Server::isServerSocket(int fd) {
  return std::any_of(_serverSockets.begin(), _serverSockets.end(),
                     [fd](std::shared_ptr<Socket> socket) {
                       return socket->getSocketFd() == fd;
                     });
}

void Server::handlePollInEvent(int fd, short& events) {
  if (isServerSocket(fd)) {
    handleNewConnection(fd);
  } else {
    auto client = _fdToClientMap.find(fd);
    if (client != _fdToClientMap.end() && client->second->isReadyForRequest()) {
      handleClientRequest(fd);
    }

    // If client is ready for response, set events to POLLOUT
    if (client != _fdToClientMap.end() &&
        client->second->isReadyForResponse()) {
      events = POLLOUT;
    }
  }
}

void Server::handlePollOutEvent(int fd, short& events) {
  auto client = _fdToClientMap.find(fd);
  auto it =
      std::find_if(_clients.begin(), _clients.end(),
                   [fd](Client* client) { return client->getFd() == fd; });
  if (client != _fdToClientMap.end() && client->second->isReadyForResponse()) {
    if (!client->second->execute()) {
      cleanupClient(it);
      return;
    }
    events = POLLIN;
  }
}

void Server::handlePollHupEvent(int fd) {
  Log::getInstance().debug("Connection closed by client");
  auto it =
      std::find_if(_clients.begin(), _clients.end(),
                   [fd](Client* client) { return client->getFd() == fd; });
  if (it != _clients.end()) {
    cleanupClient(it);
  }
}

void Server::handlePollErrEvent(int fd) {
  Log::getInstance().error("Error on socket " + std::to_string(fd) + ": " +
                           std::strerror(errno));
  auto it =
      std::find_if(_clients.begin(), _clients.end(),
                   [fd](Client* client) { return client->getFd() == fd; });
  if (it != _clients.end()) {
    cleanupClient(it);
  }
}

void Server::cleanupClient(std::vector<Client*>::iterator& it) {
  _fdToClientMap.erase((*it)->getFd());
  _pollManager.removeSocket((*it)->getFd());
  close((*it)->getFd());
  delete *it;
  _clients.erase(it);
}

void Server::buildPortToServer() {
  auto servers = _config->_httpContext._serverContext;
  for (auto server : servers) {
    for (auto serverPort : server->_listenValue) {
      _portToServerContextMap[serverPort] = server;
    }
  }
}

void Server::handleNewConnection(int serverFd) {
  struct sockaddr_in address;
  int serverPort = _pollManager._fdToPortMap[serverFd];
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
    Log::getInstance().debug("New connection on port " +
                             std::to_string(serverPort));
    auto serverContext = _portToServerContextMap[serverPort];
    Client* new_client = new Client(new_socket, serverContext);
    _clients.push_back(new_client);
    _pollManager.addSocket(new_socket, serverPort);
    _fdToClientMap[new_socket] = new_client;
  }
}
