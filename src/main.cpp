#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "../include/networking/Server.hpp"

#define PORT 8080

int main() {
  net::Server server(PORT);
  server.run();
  return 0;
}
