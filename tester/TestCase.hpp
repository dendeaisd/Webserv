#include <iostream>
#include <string>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

class TestCase {
 public:
  TestCase();
  ~TestCase();
  template <typename T>
  void assertEqual(const T &actual, const T &expected) {
    if (actual == expected) {
      printSuccess("PASS");
      std::cout << std::endl;
      TestCase::_passed++;
    } else {
      printFailure("FAIL");
      std::cerr << " (Expected: " << expected;
      std::cerr << ", Actual: " << actual << ")" << std::endl;
      TestCase::_failed++;
    }
}
 protected:
  static int _passed;
  static int _failed;
 private:
  void printSuccess(std::string msg);
  void printFailure(std::string msg);
};
