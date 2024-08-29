#include "../include/request/HttpRequest.hpp"
#include "../include/request/HttpRequestParser.hpp"
#include "TestCase.hpp"

class TestRequest : public TestCase {
 private:
  HttpRequest request;

  void testGET();
  void testPOST();
  void testPostWithFile();
  void testPostFormData();
  void testPUT();
  void testDELETE();

 public:
  TestRequest();
};
