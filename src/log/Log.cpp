#include "../../include/log/Log.hpp"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

std::string getCurrentDatetime() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::tm* now_tm = std::localtime(&now_c);
  std::stringstream ss;
  ss << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");
  return ss.str();
}

log::Log& log::Log::getInstance() {
  static Log instance;
  return instance;
}

void log::Log::log(const std::string& message) {
  std::lock_guard<std::mutex> lock(mutex_);
  writeLog(message);
}

void log::Log::error(const std::string& message) {
  std::lock_guard<std::mutex> lock(mutex_);
  writeError(message);
}

void log::Log::warning(const std::string& message) {
  std::lock_guard<std::mutex> lock(mutex_);
  writeLog("[ WARNING ] " + message);
}

void log::Log::info(const std::string& message) {
  std::lock_guard<std::mutex> lock(mutex_);
  writeLog("[ INFO ] " + message);
}

void log::Log::debug(const std::string& message) {
  std::lock_guard<std::mutex> lock(mutex_);
  writeLog("[ DEBUG ] " + message);
}

void log::Log::writeLog(const std::string& message) {
  std::ofstream file(logFile_, std::ios::app);
  if (!file.is_open()) {
    std::cerr << "Failed to open log file" << std::endl;
    return;
  }
  file << getCurrentDatetime() << " " << message << std::endl;
  file.close();
}

void log::Log::writeError(const std::string& message) {
  std::ofstream file(errorFile_, std::ios::app);
  if (!file.is_open()) {
    std::cerr << "Failed to open error log file" << std::endl;
    return;
  }
  file << getCurrentDatetime() << " [ ERROR ] ";
  file << " " << message << std::endl;
  file.close();
}
