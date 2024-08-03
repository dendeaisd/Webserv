#include "TestCase.hpp"
#include "../include/request/HttpRequest.hpp"
#include "../include/request/HttpRequestParser.hpp"

class TestRequest : public TestCase {
 private:
  HttpRequest request;

  void testGET();
  void testPOST();
//   void testPUT();
//   void testDELETE();

 public:
  TestRequest();
};
