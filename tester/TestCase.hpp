#include <exception>
#include <iostream>
#include <string>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"
#ifndef HALT_ON_FAILURE
#define HALT_ON_FAILURE 0
#endif

class TestCase {
 public:
  template <typename T>
  void assertEqual(const T &actual, const T &expected) {
    std::cout << "Actual: " << actual << ", Expected: " << expected << " - ";
    try {
      if (HALT_ON_FAILURE == 1)
        assert(actual == expected);
      else if (actual != expected)
        throw std::exception();
      printSuccess("PASS");
      std::cout << std::endl;
      TestCase::_passed++;
    } catch (const std::exception &e) {
      std::cerr << e.what() << std::endl;
      printFailure("FAIL");
      std::cerr << " (Expected: " << expected;
      std::cerr << ", Actual: " << actual << ")" << std::endl;
      TestCase::_failed++;
    }
  }

 protected:
  static int _passed;
  static int _failed;

  TestCase();
  ~TestCase();

 private:
  void printSuccess(std::string msg);
  void printFailure(std::string msg);
};
