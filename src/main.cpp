#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>

#include "../include/networking/PollManager.hpp"
#include "../include/networking/PollManagerExceptions.hpp"
#include "../include/networking/Socket.hpp"
using namespace net;

bool testAddValidSocket() {
  PollManager manager;
  int mockSocketFd = 123;

  try {
    manager.addSocket(mockSocketFd);
  } catch (const invalidFd& e) {
    std::cerr << "Error adding valid socket: " << e.what() << std::endl;
    return false;
  }
  // Check if size increased
  return manager.getPollSize() == 1;
}

// Test adding a valid socket with specific events (e.g., POLLOUT)
bool testAddValidSocketWithEvents() {
  PollManager manager;
  int mockSocketFd = 456;

  try {
    manager.addSocket(mockSocketFd, POLLOUT); // Specify POLLOUT events
  } catch (const invalidFd& e) {
    std::cerr << "Error adding valid socket with events: " << e.what()
              << std::endl;
    return false;
  }

  return manager.getPollSize() == 1 && manager.getPollFd(0).events == POLLOUT;
}

bool testAddInvalidSocket() {
  PollManager manager;
  int invalidSocketFd = -1;

  try {
    manager.addSocket(invalidSocketFd);
    return false; // It should have thrown an exception
  } catch (const invalidFd& e) {
    return true;
  }
}

bool testRemoveValidSocket() {
  PollManager manager;
  int mockSocketFd = 456;
  manager.addSocket(mockSocketFd); // Add it first to remove it

  try {
    manager.removeSocket(mockSocketFd);
  } catch (const pollManagerException& e) {
    std::cerr << "Error removing valid socket: " << e.what() << std::endl;
    return false;
  }

  return manager.getPollSize() == 0; // Check if size decreased
}

bool testRemoveInvalidSocket() {
  PollManager manager;
  int notAddedSocketFd = 789;

  try {
    manager.removeSocket(notAddedSocketFd);
    return false; // It should have thrown an exception
  } catch (const pollManagerException& e) {
    return true;
  }
}

// Test pollSockets with no sockets added
bool testPollEmptySockets() {
  PollManager manager;

  try {
    manager.pollSockets(100); // Should throw an exception
    return false;
  } catch (const pollManagerException& e) {
    return true;
  }
}

// Test pollSockets with a timeout (this will likely return 0 immediately)
bool testPollTimeout() {
  PollManager manager;
  int sockets[2];

  // Create a pair of connected sockets
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
    std::cerr << "Error creating socket pair: " << std::strerror(errno)
              << std::endl;
    return false;
  }

  manager.addSocket(sockets[0]);
  try {
    int numReady = manager.pollSockets(10); // Small timeout
    close(sockets[0]);                      // Close the sockets after testing
    close(sockets[1]);
    return numReady == 0;
  } catch (const pollFailed& e) {
    std::cerr << "Error polling sockets with timeout: " << e.what()
              << std::endl;
    close(sockets[0]);
    close(sockets[1]);
    return false;
  }
}

// Test getting a valid pollfd
bool testGetValidPollFd() {
  PollManager manager;
  int mockSocketFd = 456;
  manager.addSocket(mockSocketFd);

  try {
    struct pollfd& pfd = manager.getPollFd(0);
    return pfd.fd == mockSocketFd;
  } catch (const pollManagerException& e) {
    std::cerr << "Error getting valid pollfd: " << e.what() << std::endl;
    return false;
  }
}

// Test getting an invalid pollfd (index out of range)
bool testGetInvalidPollFd() {
  PollManager manager;
  try {
    manager.getPollFd(0); // Should throw an exception
    return false;
  } catch (const pollManagerException& e) {
    return true;
  }
}

/****FAILED_TEST*****/

// Test adding a socket that's already in the poll set
bool testAddDuplicateSocket() {
  PollManager manager;
  int mockSocketFd = 1024;

  std::cout << "Adding socket for the first time with POLLIN: " << mockSocketFd
            << std::endl;          // Debug
  manager.addSocket(mockSocketFd); // Use default events (POLLIN)

  std::cout << "Attempting to add the same socket again with POLLIN: "
            << mockSocketFd << std::endl; // Debug

  try {
    manager.addSocket(mockSocketFd); // Should throw duplicateSocket exception

    std::cerr << "Error: Adding duplicate socket with same events did NOT "
                 "throw an exception!"
              << std::endl;
    return false;
  } catch (const duplicateSocket& e) { // Catch the specific exception
    std::cout << "Caught expected duplicateSocket exception: " << e.what()
              << std::endl;
    return true;
  } catch (const pollManagerException&
               e) { // Catch any other unexpected PollManager exception
    std::cerr << "Error: Caught unexpected exception: " << e.what()
              << std::endl;
    return false;
  }
}

// Test adding a duplicate socket with different events (should NOT throw an
// exception)
bool testAddDuplicateSocketWithDifferentEvents() {
  PollManager manager;
  int mockSocketFd = 1024;

  manager.addSocket(mockSocketFd, POLLIN); // Add with POLLIN

  try {
    manager.addSocket(mockSocketFd, POLLOUT); // Add again with POLLOUT
  } catch (const pollManagerException& e) {
    std::cerr << "Error: Adding duplicate socket with different events threw "
                 "an exception: "
              << e.what() << std::endl;
    return false;
  }

  return manager.getPollSize() == 2; // Check if both were added
}

// Test pollSockets with multiple sockets, some ready and some not
bool testPollMultipleSockets() {
  // Create server socket
  Socket server(AF_INET, SOCK_STREAM, 0);
  if (!server.Bind(8080, "127.0.0.1")) {
    std::cerr << "Error binding server socket: " << strerror(errno)
              << std::endl;
    return false;
  }
  if (!server.Listen(1)) {
    std::cerr << "Error listening on server socket: " << strerror(errno)
              << std::endl;
    return false;
  }

  // Connect the first client socket
  Socket client(AF_INET, SOCK_STREAM, 0);
  sockaddr_in serverAddress = server.getServerAddress();
  if (connect(client.getFd(), (struct sockaddr*)&serverAddress,
              sizeof(serverAddress)) == -1) {
    std::cerr << "Error connecting client socket: " << strerror(errno)
              << std::endl;
    return false;
  }

  // Accept the connection on the server side
  Socket acceptedClient = server.Accept();
  if (acceptedClient.getFd() == -1) {
    std::cerr << "Error accepting client connection: " << strerror(errno)
              << std::endl;
    return false;
  }

  // Debugging: Check accepted client socket state
  int error = 0;
  socklen_t len = sizeof(error);
  if (getsockopt(acceptedClient.getFd(), SOL_SOCKET, SO_ERROR, &error, &len) <
      0) {
    std::cerr << "Error getting socket option (SO_ERROR): " << strerror(errno)
              << std::endl;
    return false;
  } else if (error != 0) {
    std::cerr << "Socket error after accept: " << strerror(error) << std::endl;
    return false;
  }

  // Send data to the client socket (to make it POLLIN ready)
  const char* message = "Hello from server!";
  ssize_t bytesSent = send(client.getFd(), message, strlen(message),
                           0); // Send from client to server
  if (bytesSent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
    std::cerr << "Error sending data: " << strerror(errno) << std::endl;
    return false;
  }

  // Add the accepted client socket to the poll manager
  net::PollManager manager;
  manager.addSocket(acceptedClient.getFd(),
                    POLLIN); // Monitor for POLLIN event

  // Poll the sockets
  try {
    int numReady = manager.pollSockets(1000); // Adjust the timeout as needed
    // Check if the number of ready sockets is correct
    if (numReady < 1) {
      std::cerr << "Expected at least one ready socket, but found " << numReady
                << std::endl;
      return false;
    }

    pollfd readyFd = manager.getPollFd(0); // Get the first ready socket
    if ((readyFd.revents & POLLIN) == 0) {
      std::cerr << "Expected POLLIN event, but found " << readyFd.revents
                << std::endl;
      return false;
    }

    // Read the data to confirm it's there
    char buffer[256];
    ssize_t bytesRead =
        recv(acceptedClient.getFd(), buffer, sizeof(buffer) - 1, 0);
    if (bytesRead < 0) {
      std::cerr << "Error reading data: " << strerror(errno) << std::endl;
      return false;
    }
    buffer[bytesRead] = '\0';
    std::cout << "Received message: " << buffer << std::endl;

    return true;
  } catch (const net::pollFailed& e) {
    std::cerr << "Polling failed: " << e.what() << std::endl;
    return false;
  }
}

// int main() {
//   bool allTestsPassed = true;

//   if (!testAddValidSocket()) {
//     std::cout << "testAddValidSocket failed" << std::endl;
//     allTestsPassed = false;
//   }
//   if (!testAddValidSocketWithEvents()) {
//     std::cout << "testAddValidSocketWithEvents failed" << std::endl;
//   }

//   if (!testAddInvalidSocket()) {
//     std::cout << "testAddInvalidSocket failed" << std::endl;
//     allTestsPassed = false;
//   }

//   if (!testRemoveValidSocket()) {
//     std::cout << "testRemoveValidSocket failed" << std::endl;
//     allTestsPassed = false;
//   }
//   if (!testRemoveInvalidSocket()) {
//     std::cout << "testRemoveInvalidSocket failed" << std::endl;
//     allTestsPassed = false;
//   }

//   if (!testPollEmptySockets()) {
//     std::cout << "testPollEmptySockets failed" << std::endl;
//     allTestsPassed = false;
//   }

//   if (!testPollTimeout()) {
//     std::cout << "testPollTimeout failed" << std::endl;
//     allTestsPassed = false;
//   }

//   if (!testGetValidPollFd()) {
//     std::cout << "testGetValidPollFd failed" << std::endl;
//     allTestsPassed = false;
//   }
//   if (!testGetInvalidPollFd()) {
//     std::cout << "testGetInvalidPollFd failed" << std::endl;
//     allTestsPassed = false;
//   }

//   if (!testAddDuplicateSocket()) {
//     std::cout << "testAddDuplicateSocket failed" << std::endl;
//     allTestsPassed = false;
//   }

//   if (!testAddDuplicateSocketWithDifferentEvents()) {
//     std::cout << "testAddDuplicateSocketWithDifferentEvents failed"
//               << std::endl;
//     allTestsPassed = false;
//   }

//   if (!testPollMultipleSockets()) {
//     std::cout << "testPollMultipleSockets failed" << std::endl;
//     allTestsPassed = false;
//   }

//   if (allTestsPassed) {
//     std::cout << "All tests passed!" << std::endl;
//     return 0;
//   } else {
//     std::cout << "Some tests failed." << std::endl;
//     return 1;
//   }
// }
// #include <iostream>
// #include <stdexcept>
//
// #include "../include/networking/Server.hpp"
//
// int main() {
//   try {
//     Server server(8080);
//
//     // Start the server loop
//     server.run();
//   } catch (const std::exception& e) {
//     std::cerr << "Server error: " << e.what() << std::endl;
//     return 1;
//   }
//
//   return 0;
// }

#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

const char* HTTP_RESPONSE =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "Hello, World!";

int main() {
  int server_fd, new_socket, opt = 1;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[BUFFER_SIZE];
  struct pollfd fds[MAX_CLIENTS + 1];
  int nfds = 1; // number of file descriptors

  // Create socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Set socket options
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt,
                 sizeof(opt)) < 0) {
    perror("setsockopt");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  // Bind socket to port
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);
  if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
    perror("bind failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  // Start listening
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  // Initialize pollfd structure
  fds[0].fd = server_fd;
  fds[0].events = POLLIN;

  while (true) {
    int poll_count = poll(fds, nfds, -1);

    if (poll_count < 0) {
      perror("poll error");
      close(server_fd);
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nfds; i++) {
      if (fds[i].revents & POLLIN) {
        if (fds[i].fd == server_fd) {
          // New incoming connection
          if ((new_socket = accept(server_fd, (struct sockaddr*)&address,
                                   (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
          }

          // Add new socket to pollfd array
          if (nfds < MAX_CLIENTS + 1) {
            fds[nfds].fd = new_socket;
            fds[nfds].events = POLLIN;
            nfds++;
          } else {
            std::cerr << "Too many clients, rejecting connection." << std::endl;
            close(new_socket);
          }
        } else {
          // Data received from client
          int client_fd = fds[i].fd;
          int bytes_read = read(client_fd, buffer, BUFFER_SIZE);

          if (bytes_read <= 0) {
            // Connection closed or error
            close(client_fd);
            fds[i] = fds[nfds - 1]; // Replace current with last
            nfds--;
          } else {
            // Respond with HTTP response
            buffer[bytes_read] = '\0';
            std::cout << "Received: " << buffer << std::endl;
            send(client_fd, HTTP_RESPONSE, strlen(HTTP_RESPONSE), 0);
            close(client_fd);
            fds[i] = fds[nfds - 1]; // Replace current with last
            nfds--;
          }
        }
      }
    }
  }

  // Close server socket
  close(server_fd);
  return 0;
}
