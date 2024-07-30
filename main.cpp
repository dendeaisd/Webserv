#include "include/request/HttpRequestParser.hpp"
#include <iostream>

int main()
{
    std::string getRequest = "GET / HTTP/1.1\r\nHost: localhost:8080\r\nConnection: keep-alive\r\n\r\n";
    HttpRequestParser parser(getRequest);
    HttpRequest req = parser.getHttpRequest();
    std::cout << req.getMethod() << std::endl;
    std::cout << req.getUri() << std::endl;
    std::cout << req.getHttpVersion() << std::endl;
    std::cout << req.getHost() << std::endl;
    std::cout << req.getPort() << std::endl;

    std::string postRequest = "POST / HTTP/1.1\r\nHost: localhost:8080\r\nConnection: keep-alive\r\nContent-Length: 5\r\n\r\nHello";
    HttpRequestParser parser2(postRequest);
    HttpRequest req2 = parser2.getHttpRequest();    
    std::cout << req2.getMethod() << std::endl;
    std::cout << req2.getUri() << std::endl;
    std::cout << req2.getHttpVersion() << std::endl;
    std::cout << req2.getHost() << std::endl;
    std::cout << req2.getPort() << std::endl;
    std::cout << req2.getBody() << std::endl;
    std::cout << req2.toString() << std::endl;
    return 0;
}
