#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include "./include/cgi/CGIFileManager.hpp"
#include "./include/log/Log.hpp"
#include "./include/networking/Server.hpp"

#define PORT 8080

int main() {
  try {
    CGIFileManager::getInstance().configure("./cgi-bin");
    Server server(PORT);
    server.run();
  } catch (const socketException& e) {
    std::cerr << "Socket error: " << e.what() << std::endl;
  } catch (const pollManagerException& e) {
    std::cerr << "PollManager error:" << e.what() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << std::endl;
  }
  return 0;
}
