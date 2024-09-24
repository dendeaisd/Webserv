#ifndef SERVER_H
#define SERVER_H

#include <memory>
#include <unordered_map>

#include "Client.hpp"
#include "ConfigFile.hpp"
#include "PollManager.hpp"
#include "Socket.hpp"

class Server {
 public:
  Server(std::unique_ptr<ConfigFile>&& config);
  void run();
  ~Server();

 private:
  std::vector<std::shared_ptr<Socket>> _serverSockets;
  PollManager _pollManager;
  std::vector<Client*> _clients;
  std::unordered_map<int, Client*> _fdToClientMap;
  std::unordered_map<int, std::shared_ptr<ServerContext>>
      _portToServerContextMap;
  std::unique_ptr<ConfigFile> _config;
  std::chrono::system_clock::time_point _lastCleanup;

  void handleEvents();
  void handlePollInEvent(int fd, short& events);
  void handlePollOutEvent(int fd, short& events);
  void handlePollHupEvent(int fd);
  void handlePollErrEvent(int fd);
  void handleNewConnection(int serverFd);
  void handleClientRequest(int fd);
  void processClientRequest(Client* client);
  void buildPortToServer();
  void cleanupClient(Client* client);
  bool isServerSocket(int fd);
  int cleanupStaleClients();
};

#endif
