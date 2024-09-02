#include <string>
#include <unordered_map>

class HttpResponse {
 public:
  HttpResponse(int status = -1);
  ~HttpResponse();
  void setStatusCode(int statusCode);
  void setBody(std::string body);
  void setHeader(std::string key, std::string value);
  std::string getResponse();
  void setContentType(std::string contentType);
  void setVersion(std::string version);
  void setFile(std::string path);
  void setFile(std::string path, std::string contentType);
  void setFile(std::string path, std::string contentType,
               std::string disposition);
  void setFile(std::string path, std::string contentType,
               std::string disposition, std::string filename);

  bool sendResponse(int fd);

 private:
  std::string _version;
  int _statusCode;
  std::string _reasonPhrase;
  std::string _body;
  std::unordered_map<std::string, std::string> _headers;
  std::string _contentType;
  std::string _file;

  std::string loadFile();
  void sendHeaders(int fd);
};