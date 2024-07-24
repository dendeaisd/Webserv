#include <arpa/inet.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include "../include/networking/PollManager.hpp"
#include "../include/networking/PollManagerExceptions.hpp"
#include <cassert>  // For assertions
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

    return manager.getPollSize() == 1; // Check if size increased
}

// Test adding a valid socket with specific events (e.g., POLLOUT)
bool testAddValidSocketWithEvents() {
    PollManager manager;
    int mockSocketFd = 456; 

    try {
        manager.addSocket(mockSocketFd, POLLOUT); // Specify POLLOUT events
    } catch (const invalidFd& e) {
        std::cerr << "Error adding valid socket with events: " << e.what() << std::endl;
        return false;
    }

    return manager.getPollSize() == 1 && manager.getPollFd(0).events == POLLOUT; 
    // Check size and correct event
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
    int mockSocketFd = socket(AF_INET, SOCK_STREAM, 0); // Create a real socket
    if (mockSocketFd == -1) {
        std::cerr << "Error creating socket: " << std::strerror(errno) << std::endl;
        return false;
    }

    manager.addSocket(mockSocketFd);
    try {
        int numReady = manager.pollSockets(10); // Small timeout
        close(mockSocketFd); // Close the socket after testing
        return numReady == 0; 
    } catch (const pollFailed& e) {
        std::cerr << "Error polling sockets with timeout: " << e.what() << std::endl;
        close(mockSocketFd);
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

    std::cout << "Adding socket for the first time with POLLIN: " << mockSocketFd << std::endl; // Debug
    manager.addSocket(mockSocketFd); // Use default events (POLLIN)

    std::cout << "Attempting to add the same socket again with POLLIN: " << mockSocketFd << std::endl; // Debug

    try {
        manager.addSocket(mockSocketFd);  // Should throw duplicateSocket exception

        std::cerr << "Error: Adding duplicate socket with same events did NOT throw an exception!" << std::endl;
        return false;
    } catch (const duplicateSocket& e) {  // Catch the specific exception
        std::cout << "Caught expected duplicateSocket exception: " << e.what() << std::endl;
        return true; 
    } catch (const pollManagerException& e) {  // Catch any other unexpected PollManager exception
        std::cerr << "Error: Caught unexpected exception: " << e.what() << std::endl;
        return false;
    }
}


// Test adding a duplicate socket with different events (should NOT throw an exception)
bool testAddDuplicateSocketWithDifferentEvents() {
    PollManager manager;
    int mockSocketFd = 1024;  

    manager.addSocket(mockSocketFd, POLLIN);  // Add with POLLIN

    try {
        manager.addSocket(mockSocketFd, POLLOUT);  // Add again with POLLOUT
    } catch (const pollManagerException& e) {
        std::cerr << "Error: Adding duplicate socket with different events threw an exception: " << e.what() << std::endl;
        return false;
    }

    return manager.getPollSize() == 2; // Check if both were added
}

// Test pollSockets with multiple sockets, some ready and some not
/*bool testPollMultipleSockets() {
    PollManager manager;
    std::vector<int> socketFds;

    // Create a few sockets (some will be ready, some won't)
    for (int i = 0; i < 5; ++i) {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1) {
            std::cerr << "Error creating socket: " << std::strerror(errno) << std::endl;
            // Clean up already created sockets
            for (int fd : socketFds) {
                close(fd);
            }
            return false;
        }
        socketFds.push_back(fd);
        manager.addSocket(fd);
    }

    // Make the first socket "ready" (you'd normally have some network activity)
    struct sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080); // Just an example port
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr); 
    if (connect(socketFds[0], (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        std::cerr << "Error connecting socket: " << std::strerror(errno) << std::endl;
        // Clean up sockets
        for (int fd : socketFds) {
            close(fd);
        }
        return false;
    }

    try {
        int numReady = manager.pollSockets(100); // Should be at least 1

        // Clean up sockets
        for (int fd : socketFds) {
            close(fd);
        }

        return numReady >= 1;  
    } catch (const pollFailed& e) {
        std::cerr << "Error polling sockets: " << e.what() << std::endl;
        // Clean up sockets
        for (int fd : socketFds) {
            close(fd);
        }
        return false;
    }
}*/




int main() {
    // Run the tests
    bool allTestsPassed = true;

    /*if (!testAddValidSocket()) {
        std::cout << "testAddValidSocket failed" << std::endl;
        allTestsPassed = false;
    }
    */if (!testAddValidSocketWithEvents()) {
        std::cout << "testAddValidSocketWithEvents failed" << std::endl;
    }/*

    if (!testAddInvalidSocket()) {
        std::cout << "testAddInvalidSocket failed" << std::endl;
        allTestsPassed = false;
    }

    // Test removeSocket
    if (!testRemoveValidSocket()) {
        std::cout << "testRemoveValidSocket failed" << std::endl;
        allTestsPassed = false;
    }
    if (!testRemoveInvalidSocket()) {
        std::cout << "testRemoveInvalidSocket failed" << std::endl;
        allTestsPassed = false;
    }

    // Test pollSockets
    if (!testPollEmptySockets()) {
        std::cout << "testPollEmptySockets failed" << std::endl;
        allTestsPassed = false;
    }
    if (!testPollTimeout()) {
        std::cout << "testPollTimeout failed" << std::endl;
        allTestsPassed = false;
    }

    // Test getPollFd
    if (!testGetValidPollFd()) {
        std::cout << "testGetValidPollFd failed" << std::endl;
        allTestsPassed = false;
    }
    if (!testGetInvalidPollFd()) {
        std::cout << "testGetInvalidPollFd failed" << std::endl;
        allTestsPassed = false;
    }

    if (!testAddDuplicateSocket()) {
        std::cout << "testAddDuplicateSocket failed" << std::endl;
        allTestsPassed = false;
    }*/
    
    if (!testAddDuplicateSocketWithDifferentEvents()) {
      std::cout << "testAddDuplicateSocketWithDifferentEvents failed" << std::endl;
      allTestsPassed = false;
  }
    /*if (!testPollMultipleSockets()) {
        std::cout << "testPollMultipleSockets failed" << std::endl;
        allTestsPassed = false;
    }*/

    if (allTestsPassed) {
        std::cout << "All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "Some tests failed." << std::endl;
        return 1;
    }
}
