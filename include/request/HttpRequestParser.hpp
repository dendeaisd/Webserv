// also exception, maybe it should be included in the file that
// is actually using the header ?
#include <exception>
#include <map>
#include <set>
#include <string>
// you are not using the sstream header direcly anywhere here
#include <sstream>

#include "../FileUpload.hpp"  // path was not correct, i changed it
#include "HttpRequestEnums.hpp"

// This class seems to have to much responsabilities in my
// opinion(parsing, storing the parsed data and validation).
// I would split this into smaller more focused classes
// so we can have a tool for each specific job
class HttpRequestParser {
 public:
  HttpRequestParseStatus status;
  bool hasFile;

  HttpRequestParser(std::string request);
  ~HttpRequestParser();
  // Getters and setters
  std::string getMethod();
  std::string getUri();
  std::string getHttpVersion();
  std::string getHost();
  std::string getPort();
  std::string getBody();
  std::string getHeader(std::string header);
  std::map<std::string, std::string> getHeaders();
  void setMethod(std::string method);
  void setUri(std::string uri);
  void setHttpVersion(std::string httpVersion);
  void setHost(std::string host);
  void setPort(std::string port);
  void setBody(std::string body);
  void setHeader(std::string header, std::string value);
  void setHeaders(std::map<std::string, std::string> headers);
  std::string toString();

 private:
  std::string raw;
  std::string request;
  std::string method;
  HttpRequestMethod httpRequestMethod;
  HttpRequestVersion httpProtocolVersion;
  std::string uri;
  std::string httpVersion;
  std::string host;
  std::string port;
  std::string body;
  std::string subDomain;
  std::string domain;
  FileUpload file;
  std::map<std::string, std::string> headers;
  std::map<std::string, std::string> queryParams;
  std::map<std::string, HttpRequestMethod> httpRequestMethodMap = {
      {"GET", HttpRequestMethod::GET},
      {"POST", HttpRequestMethod::POST},
      {"PUT", HttpRequestMethod::PUT},
      {"DELETE", HttpRequestMethod::DELETE},
      {"HEAD", HttpRequestMethod::HEAD},
      {"OPTIONS", HttpRequestMethod::OPTIONS},
      {"PATCH", HttpRequestMethod::PATCH},
      {"TRACE", HttpRequestMethod::TRACE},
      {"CONNECT", HttpRequestMethod::CONNECT},
  };
  std::map<std::string, HttpRequestVersion> httpRequestVersionMap = {
      {"HTTP/1.0", HttpRequestVersion::HTTP_1_0},
      {"HTTP/1.1", HttpRequestVersion::HTTP_1_1},
      {"HTTP/2.0", HttpRequestVersion::HTTP_2_0},
  };
  std::set<std::string> requiredHeaders1_1 = {"Host"};
  std::set<std::string> headerSet = {
      "Host",        // Required
      "User-Agent",  // Basically the browser or client library that is making
                     // the request
      "Accept",      // The media types that the client can understand
      "Accept-Language",  // The languages that the client can understand
      "Accept-Encoding",  // The encodings that the client can understand (e.g.
                          // gzip)
      "Connection",  // Whether the client wants to keep the connection open or
                     // not (e.g. keep-alive)
      "Upgrade-Insecure-Requests",  // Whether the client wants to upgrade to
                                    // HTTPS
      "Cache-Control",  // Whether the client wants to cache the response or not
      "Content-Type",   // The media type of the request body (e.g.
                        // application/json)
      "Content-Length",  // The length of the request body
      "Origin",      // The origin of the request (e.g. http://localhost:8080)
      "Referer",     // The URL of the page that made the request (e.g.
                     // http://localhost:8080/index.html)
      "Cookie",      // The cookies that the client has sent to the server
      "Set-Cookie",  // The cookies that the server has sent to the client
      "Authorization",  // The credentials that the client has sent to the
                        // server
  };
  void parse();
  void parseRequestLine(char *requestLine);
  void parseHeaders();
  void parseBody();
  void parseQueryParams(std::string query);
  bool validateRequestMethod();
  bool validateHttpVersion();
};
