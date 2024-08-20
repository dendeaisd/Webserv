#include <string>

class Helpers {
  Helpers() = delete;
  ~Helpers() = delete;

  Helpers(const Helpers &) = delete;
  Helpers &operator=(const Helpers &) = delete;

  Helpers(Helpers &&) = delete;
  Helpers &operator=(Helpers &&) = delete;

 public:
  static std::string getExtension(const std::string &path);
  static std::string getContentType(const std::string &extension);
  static bool boundaryUpcoming(std::stringstream &ss,
                               const std::string &boundary);
  static int countRemainingLines(std::stringstream &ss);
  static std::string getFormKeyIfExists(std::string data);
  static std::string getFilenameIfExists(std::string data);
};