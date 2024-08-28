
#pragma once
#ifndef _EVENT_HPP_
#define _EVENT_HPP_
#include <unordered_map>
#include <memory>

#include "cgi/CGI.hpp"

class Event {
 public:
  static Event &getInstance() {
    static Event instance;
    return instance;
  }
  void addEvent(int fd, std::shared_ptr<CGI> cgi) { _events[fd] = cgi; }

  void removeEvent(int fd) {
    _events.erase(fd);
  }

  std::shared_ptr<CGI> getEvent(int fd) { return _events[fd]; }

  bool hasEvent(int fd) { return _events.find(fd) != _events.end(); }

  std::unordered_map<int, std::shared_ptr<CGI>> &getEvents() { return _events; }

 private:
  Event() {}
  Event(const Event &) = delete;
  Event &operator=(const Event &) = delete;
  std::unordered_map<int, std::shared_ptr<CGI>> _events;
};

#endif  // _EVENT_HPP_