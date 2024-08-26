
#pragma once
#ifndef _EVENT_HPP_
#define _EVENT_HPP_
#include <unordered_map>

#include "cgi/CGI.hpp"

class Event {
 public:
  static Event &getInstance() {
    static Event instance;
    return instance;
  }
  void addEvent(int fd, CGI *cgi) { _events[fd] = cgi; }

  void removeEvent(int fd) {
    CGI *cgi = _events[fd];
    delete cgi;
    _events.erase(fd);
  }

  CGI *getEvent(int fd) { return _events[fd]; }

  bool hasEvent(int fd) { return _events.find(fd) != _events.end(); }

  std::unordered_map<int, CGI *> &getEvents() { return _events; }

 private:
  Event() {}
  Event(const Event &) = delete;
  Event &operator=(const Event &) = delete;
  std::unordered_map<int, CGI *> _events;
};

#endif  // _EVENT_HPP_