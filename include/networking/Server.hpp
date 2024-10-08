#ifndef SERVER_H
#define SERVER_H

#include <csignal>
#include <memory>
#include <unordered_map>

#include "Client.hpp"
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
  std::vector<std::shared_ptr<Client>> _clients;
  std::unordered_map<int, std::shared_ptr<Client>> _fdToClientMap;
  std::unordered_map<int, std::shared_ptr<ServerContext>>
      _portToServerContextMap;
  std::unique_ptr<ConfigFile> _config;
  std::chrono::system_clock::time_point _lastCleanup;
  size_t _processedRequests;
  size_t _requestCount;

  void handleEvents();
  void handlePollInEvent(int fd, short& events);
  void handlePollOutEvent(int fd, short& events);
  void handlePollHupEvent(int fd);
  void handlePollErrEvent(int fd);
  void handleNewConnection(int serverFd);
  bool handleClientRequest(int fd);
  bool processClientRequest(std::shared_ptr<Client> client);
  void buildPortToServer();
  void cleanupClient(std::shared_ptr<Client> client);
  bool isServerSocket(int fd);
  int cleanupStaleClients();
  void shutdown();
};

#endif
