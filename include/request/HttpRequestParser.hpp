// also exception, maybe it should be included in the file that
// is actually using the header ?
#include <exception>
#include <map>
#include <set>
#include <string>
// you are not using the sstream header direcly anywhere here
#include <sstream>
#include <iostream>

// #include "../FileUpload.hpp"  // path was not correct, i changed it
#include "HttpRequestEnums.hpp"
#include "HttpMaps.hpp"
#include "HttpRequest.hpp"

// This class seems to have to much responsabilities in my
// opinion(parsing, storing the parsed data and validation).
// I would split this into smaller more focused classes
// so we can have a tool for each specific job
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
