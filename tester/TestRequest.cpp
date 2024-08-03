#include "TestRequest.hpp"

TestRequest::TestRequest() {
  testGET();
  testPOST();
//   testPUT();
//   testDELETE();
}


void TestRequest::testGET() {
  	std::string getRequest = "GET / HTTP/1.1\r\nHost: localhost:8080\r\nConnection: keep-alive\r\n\r\n";
    HttpRequestParser parser(getRequest);
    request = parser.getHttpRequest();
    assertEqual<std::string>(request.getMethod(), "GET");
	assertEqual<std::string>(request.getUri(), "/");
	assertEqual<std::string>(request.getHttpVersion(), "HTTP/1.1");
	assertEqual<std::string>(request.getHost(), "localhost");
	assertEqual<int>(request.getPort().length(), 4);
	assertEqual<std::string>(request.getPort(), "8080");
	assertEqual<std::string>(request.getBody(), "");
}

void TestRequest::testPOST() {
  	std::string postRequest = "POST / HTTP/1.1\r\nHost: localhost:8080\r\nConnection: keep-alive\r\nContent-Length: 5\r\n\r\nhello";
	HttpRequestParser parser(postRequest);
	request = parser.getHttpRequest();
	assertEqual<std::string>(request.getMethod(), "POST");
	assertEqual<std::string>(request.getUri(), "/");
	assertEqual<std::string>(request.getHttpVersion(), "HTTP/1.1");
	assertEqual<std::string>(request.getHost(), "localhost");
	assertEqual<std::string>(request.getPort(), "8080");
	assertEqual<std::string>(request.getBody(), "hello");
}
