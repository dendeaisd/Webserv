#include <gtest/gtest.h>

#include "../include/networking/Server.hpp"

#define PORT 8080

// Test fixture for the Server class
class ServerTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}

  net::Server server{PORT};
};

TEST_F(ServerTest, ServerStartsCorrectly) { EXPECT_NO_THROW(server.run()); }

// Main function to run all the tests
// THIS will not run unless we remove main.cpp from src list or comment out the
// main function in main.cpp
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}