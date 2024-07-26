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

 private:
  void acceptNewClient();
  void handleClientData(int clientFd);
  void handleEvents(int numEvents);
  void handleSocketError(int fd);
  bool isValidSocket(int fd);

  int port_;
  Socket serverSocket_;
  net::PollManager pollManager_;
  std::map<int, Client> clients_;
};
} // namespace net
#endif
