// also exception, maybe it should be included in the file that
// is actually using the header ?
#include <exception>
#include <map>
#include <set>
#include <string>
// you are not using the sstream header direcly anywhere here
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>

#include "HttpMaps.hpp"
#include "HttpRequest.hpp"
#include "HttpRequestEnums.hpp"
#include "Location.hpp"
#include "ServerContext.hpp"

class HttpRequestParser {
 public:
  HttpRequestParseStatus status;
  bool hasFile;

  HttpRequestParser(std::string request, int clientFd,
                    std::shared_ptr<ServerContext> serverContext);
  HttpRequestParser();
  HttpRequest getHttpRequest();
  int parse();
  int handshake();
  int getStatusCode();
  std::string getLocation();
  ~HttpRequestParser();

 private:
  int _clientFd;
  std::shared_ptr<ServerContext> _serverContext;
  HttpRequest _request;
  std::string _raw;
  std::string _boundary;
  int _statusCode;
  std::string _location;
  std::shared_ptr<Location> _locationConfig;
  HttpFileUploadStatus currentFileUploadStatus;
  std::string currentFileUploadName;
  size_t total_read;
  bool electHandler();
  bool parseRequestLine(char *requestLine, size_t len);
  bool parseHeaders(std::stringstream &ss);
  bool parseBody(std::stringstream &ss);
  void parseFormData(std::stringstream &ss);
  bool handleFileUpload(std::stringstream &ss);
  bool handleOctetStream(std::stringstream &ss);
  bool handleMultipartFormData(std::stringstream &ss);
  void parseQueryParams(std::string query);
  bool validateHttpVersion();
  bool askForContinue();
  bool checkForTerminator(std::string line);
  bool isAllowedMethod(const std::string &method);
  bool isAllowedContentLength(size_t contentLength);
  bool isCgiRequest();
  bool isFaviconRequest();
  bool isDirectoryRequest();
  bool isFileRequest();
  bool canHaveBody();
  bool isUploadAllowed();
  void injectUploadFormIfNeeded();
  std::shared_ptr<Location> getMostRelevantLocation();
  bool parseBoundary();
  void setStatusCode(int code);
  void setLocation(std::string location);
};
