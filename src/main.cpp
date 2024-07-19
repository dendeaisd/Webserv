
#include "../include/networking/Socket.hpp"
#include "../include/networking/SocketExceptions.hpp"

#include "../include/networking/Socket.hpp"
#include "../include/networking/SocketExceptions.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

int main() {
  try {
    net::Socket serverSocket(AF_INET, SOCK_STREAM, 0);

    serverSocket.Bind(8080, "0.0.0.0");

    serverSocket.Listen(5);

    std::cout << "Server is listening on port 8080..." << std::endl;

    // Accept a single client connection (for testing purposes)
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    int clientSocketFd =
        accept(serverSocket.getFd(), (struct sockaddr *)&clientAddress,
               &clientAddressLen);

    if (clientSocketFd < 0) {
      std::cerr << "Error accepting connection: " << std::strerror(errno)
                << std::endl;
    } else {
      std::cout << "Client connected!" << std::endl;
      close(clientSocketFd); // Close the client socket
    }

  } catch (const net::SocketException &e) {
    std::cerr << "Socket exception: " << e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "General exception: " << e.what() << std::endl;
  }

  return 0;
}
