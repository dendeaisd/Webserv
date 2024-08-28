#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include "./include/log/Log.hpp"
#include "./include/networking/Server.hpp"
#include "./include/cgi/CGIFileManager.hpp"

#define PORT 8080

int main() {
  try {
	CGIFileManager::getInstance().configure("./cgi-bin");
    net::Server server(PORT);
    server.run();
  } catch (const net::socketException& e) {
    std::cerr << "Socket error: " << e.what() << std::endl;
  } catch (const net::pollManagerException& e) {
    std::cerr << "PollManager error:" << e.what() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << std::endl;
  }
  return 0;
}
