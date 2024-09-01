#ifndef SERVER_H
#define SERVER_H

#include <map>

#include "Client.hpp"
#include "PollManager.hpp"
#include "Socket.hpp"

class Server {
 public:
  Server(int port);
  void run();
  ~Server();

 private:
  Socket serverSocket_;
  PollManager pollManager_;
  std::vector<Client*> clients_;

  void handleEvents();
  void handleNewConnection();
  void handleClientRequest(int fd);
  void processClientRequest(std::vector<Client*>::iterator& it);
  void cleanupClient(std::vector<Client*>::iterator& it);
};

#endif
