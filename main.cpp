#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <vector>
#include <memory>

#include "./include/cgi/CGIFileManager.hpp"
#include "./include/log/Log.hpp"
#include "./include/networking/Server.hpp"
#include "ParseConfigFile.hpp"

#define PORT 8080

int main(int argc, char* argv[]) {
  if (argc > 2)
    std::cerr << "Usage: " << argv[0] << " [config file]" << std::endl;
  std::string configFile = argc == 2 ? argv[1] : "./configs/default.conf";
  if (access(configFile.c_str(), F_OK) == -1) {
    std::cerr << "Config file not found, default path: `./configs/default.conf`"
              << std::endl;
    return 1;
  }
  try {
    auto config = std::make_shared<ParseConfigFile>(configFile);
    auto parsed = config->getConfigFile();
    Log::getInstance().configure("DEBUG");
    if (!parsed) {
    std::cerr << "Parsed ConfigFile is null." << std::endl;
    return 1;
    }
    CGIFileManager::getInstance().configure(*parsed, "./cgi-bin");
    std::vector<int> ports = {8080, 8081, 8082};
    Server server(ports, std::move(parsed));
    server.run();
  } catch (const socketException& e) {
    std::cerr << "Socket error: " << e.what() << std::endl;
  } catch (const pollManagerException& e) {
    std::cerr << "PollManager error: " << e.what() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << std::endl;
  }
  return 0;
}
