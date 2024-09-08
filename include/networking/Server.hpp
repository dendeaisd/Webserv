#ifndef SERVER_H
#define SERVER_H

#include <unordered_map>

#include "Client.hpp"
#include "PollManager.hpp"
#include "Socket.hpp"

class Server {
 public:
  Server(std::vector<int>& ports);
  void run();
  ~Server();

 private:
  std::vector<std::shared_ptr<Socket>> _serverSockets;
  PollManager _pollManager;
  std::vector<Client*> _clients;
  std::unordered_map<int, Client*> _fdToClientMap;

  void handleEvents();
  void handleNewConnection(int serverFd);
  void handleClientRequest(int fd);
  void processClientRequest(std::vector<Client*>::iterator& it);
  void handleClientResponse(int fd);
  void cleanupClient(std::vector<Client*>::iterator& it);
  void removeCGIEvents();
};

#endif
