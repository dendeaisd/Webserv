#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <vector>

#include "./include/cgi/CGIFileManager.hpp"
#include "./include/log/Log.hpp"
#include "./include/networking/Server.hpp"

int main() {
  try {
    CGIFileManager::getInstance().configure("./cgi-bin");

    // Define the list of ports the server will listen to
    std::vector<int> ports = {8080, 8081, 8082};  // Example ports

    // Create the server with multiple ports
    Server server(ports);
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
