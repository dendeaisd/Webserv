#ifndef SERVER_H
#define SERVER_H

#include <map>

#include "Client.hpp"
#include "PollManager.hpp"
#include "Socket.hpp"

namespace net {

class Server {
 public:
  Server(int port);
  void run();
  ~Server();

 private:
  Socket serverSocket_;
  PollManager pollManager_;
  std::vector<Client *> clients_;

  void handleEvents();
  void handleNewConnection();
  void handleClientData(Client *client);
  void cleanupClients();
};

}  // namespace net
#endif
