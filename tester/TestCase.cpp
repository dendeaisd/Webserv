#include "TestCase.hpp"

int TestCase::_passed = 0;
int TestCase::_failed = 0;

TestCase::TestCase() {
  TestCase::_passed = 0;
  TestCase::_failed = 0;
}

TestCase::~TestCase() {
  printSuccess("Passed: ");
  std::cout << TestCase::_passed << ", ";
  printFailure("Failed: ");
  std::cout << TestCase::_failed << std::endl;
}

void TestCase::printSuccess(std::string msg) {
  std::cout << GREEN << msg << RESET;
}

void TestCase::printFailure(std::string msg) {
  std::cerr << RED << msg << RESET;
}
