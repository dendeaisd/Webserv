
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

Server::Server(std::unique_ptr<ConfigFile>&& config) {
  _config = std::move(config);
  buildPortToServer();
  _config->printConfigFile();

  const auto& serverContexts = _config->_httpContext._serverContext;
  std::set<std::pair<std::string, int>> addressPortSet;
  std::set<int> portsWithAddress;

  for (const auto& serverContext : serverContexts) {
    for (int port : serverContext->_listenValue) {
      if (portsWithAddress.find(port) == portsWithAddress.end()) {
        addressPortSet.insert(std::make_pair("", port));
        portsWithAddress.insert(port);
      }
    }
  }

  for (const auto& addrPortPair : addressPortSet) {
    const std::string& address = "";
    int port = addrPortPair.second;

    auto newSocket = std::make_shared<Socket>(AF_INET, SOCK_STREAM, 0);
    newSocket->setSocketOption(SOL_SOCKET, SO_REUSEADDR, 1);
    newSocket->bindSocket(port, address);
    newSocket->listenSocket(SOMAXCONN);
    newSocket->setNonBlocking();
    _pollManager.addSocket(newSocket->getSocketFd(), POLLIN, port);
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
    if (revents & POLLIN) {
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
      if (_portToServerContextMap.find(serverPort) ==
          _portToServerContextMap.end()) {
        _portToServerContextMap[serverPort] = server;
      }
    }
  }
  // print it please
  for (auto it = _portToServerContextMap.begin();
       it != _portToServerContextMap.end(); ++it) {
    Log::getInstance().debug("Port: " + std::to_string(it->first) +
                             " Server: " + it->second->_serverNameValue.at(0));
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
    std::shared_ptr<ServerContext> serverContext =
        _portToServerContextMap.find(serverPort)->second;
    Log::getInstance().debug("Server context: " +
                             serverContext->_serverNameValue.at(0));
    Client* new_client = new Client(new_socket, serverContext);
    _clients.push_back(new_client);
    _pollManager.addSocket(new_socket, POLLIN, serverPort);
    _fdToClientMap[new_socket] = new_client;
  }
}
