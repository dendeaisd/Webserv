#ifndef SERVER_H
#define SERVER_H

#include "Socket.hpp"
#include "PollManager.hpp"
#include <map>

namespace net{

class Server {
 public:
  Server(int port, const std::string& address);
  void run();

 private:
  Socket serverSocket_;
  PollManager pollManager_;
  std::map<int, bool> clientConnections_;
  
  void handleNewConnection();
  void handleClientData(int clientFd);
  void handlePollError(int fd, const std::string& errorMessage); 
};

}
#endif 
