#include "TestCase.hpp"

class TestRequest : public TestCase {
 private:
    void testHeader();
    void testBody() {}
    void testMethod() {}
    void testUrl() {}
 public:
    TestRequest();
};
