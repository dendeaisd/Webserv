#ifndef SERVER_H
#define SERVER_H

#include <map>

#include "Client.hpp"
#include "PollManager.hpp"
#include "Socket.hpp"

class Server {
 public:
  Server(std::vector<int>& ports);
  void run();
  ~Server();

 private:
  std::vector<std::shared_ptr<Socket>> serverSockets_;
  PollManager pollManager_;
  std::vector<Client*> clients_;

  void handleEvents();
  void handleNewConnection(int serverFd);
  void handleClientRequest(int fd);
  void processClientRequest(std::vector<Client*>::iterator& it);
  void cleanupClient(std::vector<Client*>::iterator& it);
  void removeCGIEvents();
};

#endif
