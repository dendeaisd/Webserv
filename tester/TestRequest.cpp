#include "TestRequest.hpp"

TestRequest::TestRequest() {
    testHeader();
}

void TestRequest::testHeader() {
    assertEqual<int>(1, 2);
    assertEqual<double>(2.1, 2.0);
    assertEqual<int>(3, 3);
    assertEqual<double>(4.0, 4.0);
    assertEqual<int>(5, 2);
}
