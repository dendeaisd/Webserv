#include "TestRequest.hpp"

TestRequest::TestRequest() {
  testGET();
  testPOST();
  testPostWithFile();
  //   testPUT();
  //   testDELETE();
}

void TestRequest::testGET() {
  std::string getRequest =
      "GET / HTTP/1.1\r\nHost: localhost:8080\r\nConnection: "
      "keep-alive\r\n\r\n";
  HttpRequestParser parser(getRequest);
  int status = parser.parse();
  assertEqual<int>(status, 200);
  request = parser.getHttpRequest();
  assertEqual<std::string>(request.getMethod(), "GET");
  assertEqual<std::string>(request.getUri(), "/");
  assertEqual<std::string>(request.getHttpVersion(), "HTTP/1.1");
  assertEqual<std::string>(request.getHost(), "localhost");
  assertEqual<int>(request.getPort().length(), 4);
  assertEqual<std::string>(request.getPort(), "8080");
  assertEqual<std::string>(request.getBody(), "");

  std::string invalidGetRequest = "GET / HTTP/1.1\r\n";
  HttpRequestParser invalidParser(invalidGetRequest);
  status = invalidParser.parse();
  assertEqual<int>(status, 400);
}

void TestRequest::testPOST() {
  std::string postRequest =
      "POST / HTTP/1.1\r\nHost: localhost:8080\r\nConnection: "
      "keep-alive\r\nContent-Length: 5\r\n\r\nhello";
  HttpRequestParser parser(postRequest);
  int status = parser.parse();
  assertEqual<int>(status, 200);
  request = parser.getHttpRequest();
  assertEqual<std::string>(request.getMethod(), "POST");
  assertEqual<std::string>(request.getUri(), "/");
  assertEqual<std::string>(request.getHttpVersion(), "HTTP/1.1");
  assertEqual<std::string>(request.getHost(), "localhost");
  assertEqual<std::string>(request.getPort(), "8080");
  assertEqual<std::string>(request.getBody(), "hello");
}

void TestRequest::testPostWithFile() {
  std::string req = "POST /submit HTTP/1.1\r\n";
  req += "Host: www.ramy.com\r\n";
  req +=
      "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) "
      "Gecko/20100101 Firefox/115.0\r\n";
  req += "Accept: */*\r\n";
  req += "Accept-Language: en-US,en;q=0.5\r\n";
  req += "Accept-Encoding: gzip, deflate, br\r\n";
  req +=
      "Content-Type: multipart/form-data; "
      "boundary=---------------------------3141592653589793238462643383\r\n";
  req += "Content-Length: 630\r\n";
  req += "\r\n";
  req += "-----------------------------3141592653589793238462643383\r\n";
  req += "Content-Disposition: form-data; name=\"name\"\r\n";
  req += "\r\n";
  req += "Ramy\r\n";
  req += "-----------------------------3141592653589793238462643383\r\n";
  req += "Content-Disposition: form-data; name=\"email\"\r\n";
  req += "\r\n";
  req += "me@ramy.com\r\n";
  req += "-----------------------------3141592653589793238462643383\r\n";
  req +=
      "Content-Disposition: form-data; name=\"attachment\"; "
      "filename=\"report.pdf\"\r\n";
  req += "Content-Type: application/pdf\r\n";
  req += "\r\n";
  req += "%PDF-1.5";
  req += "%����";
  req += "... (binary PDF data) ...";
  req += "%%EOF";
  req += "-----------------------------3141592653589793238462643383--\r\n";
  HttpRequestParser parser(req);
  int status = parser.parse();
  request = parser.getHttpRequest();
  assertEqual<int>(status, 200);
  assertEqual<std::string>(request.getMethod(), "POST");
  assertEqual<std::string>(request.getUri(), "/submit");
  assertEqual<std::string>(request.getHttpVersion(), "HTTP/1.1");
  assertEqual<std::string>(request.getHost(), "www.ramy.com");
  assertEqual<std::string>(request.getPort(), "80");
  assertEqual<std::string>(request.getBody(), "");
}
