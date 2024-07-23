#include <arpa/inet.h>
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

    std::cout << "Polling with timeout..." << std::endl; // Debugging print

    try {
        int numReady = manager.pollSockets(10); // Small timeout
        std::cout << "pollSockets returned: " << numReady << std::endl; // More debugging

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

int main() {
    // Run the tests
    bool allTestsPassed = true;

    if (!testAddValidSocket()) {
        std::cout << "testAddValidSocket failed" << std::endl;
        allTestsPassed = false;
    }
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
    // ... Add calls to other test functions ...

    if (allTestsPassed) {
        std::cout << "All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "Some tests failed." << std::endl;
        return 1;
    }
}
