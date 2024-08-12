#include "../../include/log/Log.hpp"

#include <sys/stat.h>

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#define LOG_CYCLE 60 * 60

std::string getCurrentDatetime() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::tm* now_tm = std::localtime(&now_c);
  std::stringstream ss;
  ss << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");
  return ss.str();
}

std::chrono::seconds getFileAge(const std::string& filename) {
  struct stat result;
  if (stat(filename.c_str(), &result) == 0) {
  }
  auto creationTime = std::chrono::system_clock::from_time_t(result.st_ctime);
  auto now = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::seconds>(now - creationTime);
}

log::Log& log::Log::getInstance() {
  static Log instance;
  return instance;
}

void log::Log::error(const std::string& message) {
  if (logLevel_ < LogLevel::ERROR) return;
  std::lock_guard<std::mutex> lock(mutex_);
  writeError(message);
}

void log::Log::warning(const std::string& message) {
  if (logLevel_ < LogLevel::WARNING) return;
  std::lock_guard<std::mutex> lock(mutex_);
  writeLog("[ WARNING ] " + message);
}

void log::Log::info(const std::string& message) {
  std::lock_guard<std::mutex> lock(mutex_);
  writeLog("[ INFO ] " + message);
}

void log::Log::debug(const std::string& message) {
  if (logLevel_ < LogLevel::DEBUG) return;
  std::lock_guard<std::mutex> lock(mutex_);
  writeLog("[ DEBUG ] " + message);
}

void log::Log::writeLog(const std::string& message) {
  if (logLevel_ == LogLevel::NOLOG) return;
  if (getFileAge(logFile_).count() > LOG_CYCLE) {
    moveOldLogs(logFile_);
  }
  std::ofstream file(logFile_, std::ios::app);
  if (!file.is_open()) {
    std::cerr << "Failed to open log file" << std::endl;
    return;
  }
  file << getCurrentDatetime() << " " << message << std::endl;
  file.close();
}

void log::Log::writeError(const std::string& message) {
  if (getFileAge(errorFile_).count() > LOG_CYCLE) {
    moveOldLogs(errorFile_);
  }
  std::ofstream file(errorFile_, std::ios::app);
  if (!file.is_open()) {
    std::cerr << "Failed to open error log file" << std::endl;
    return;
  }
  file << getCurrentDatetime() << " [ ERROR ] ";
  file << " " << message << std::endl;
  file.close();
}

void log::Log::moveOldLogs(const std::string& filename) {
  std::string tmp = filename;
  std::string oldFilename = tmp.erase(tmp.find_last_of('.'));
  oldFilename.append("_" + getCurrentDatetime() + ".log");
  std::rename(filename.c_str(), oldFilename.c_str());
}