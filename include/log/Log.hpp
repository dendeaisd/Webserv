#ifndef LOG_HPP
#define LOG_HPP

#include <mutex>
#include <string>

namespace log {
class Log {
  enum class LogLevel { INFO, WARNING, ERROR, DEBUG };

 public:
  static Log& getInstance();

  Log(const Log&) = delete;
  Log& operator=(const Log&) = delete;

  void log(const std::string& message);
  void error(const std::string& message);
  void warning(const std::string& message);
  void info(const std::string& message);
  void debug(const std::string& message);

 private:
  Log() = default;
  ~Log() = default;

  void writeLog(const std::string& message);
  void writeError(const std::string& message);
  void moveOldLogs(const std::string& file);

  std::mutex mutex_;
  std::string logFile_ = "logs/server.log";
  std::string errorFile_ = "logs/error.log";
  LogLevel logLevel_ = LogLevel::DEBUG;
};
}  // namespace log

#endif