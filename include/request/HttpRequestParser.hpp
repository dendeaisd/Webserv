// also exception, maybe it should be included in the file that
// is actually using the header ?
#include <exception>
#include <map>
#include <set>
#include <string>
// you are not using the sstream header direcly anywhere here
#include <iostream>
#include <sstream>

// #include "../FileUpload.hpp"  // path was not correct, i changed it
#include "HttpMaps.hpp"
#include "HttpRequest.hpp"
#include "HttpRequestEnums.hpp"

class HttpRequestParser {
 public:
  HttpRequestParseStatus status;
  bool hasFile;

  HttpRequestParser(std::string request);
  HttpRequest getHttpRequest();
  ~HttpRequestParser();

 private:
  HttpRequest request;
  std::string raw;
  void parse();
  void parseRequestLine(char *requestLine);
  void parseHeaders(std::stringstream &ss);
  void parseBody(std::stringstream &ss);
  void parseQueryParams(std::string query);
  bool validateRequestMethod();
  bool validateHttpVersion();
};
