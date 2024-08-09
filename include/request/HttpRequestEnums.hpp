#pragma once
#ifndef HTTP_REQUEST_ENUMS_HPP
#define HTTP_REQUEST_ENUMS_HPP
enum HttpRequestMethod {
  GET,
  POST,
  PUT,
  DELETE,
  HEAD,
  OPTIONS,
  PATCH,
  TRACE,
  CONNECT,
  METHOD_UNKNOWN
};

enum HttpRequestVersion { HTTP_1_0, HTTP_1_1, HTTP_2_0, VERSION_UNKNOWN };

enum HttpRequestParseStatus { NOT_PARSED, PARSED, INVALID, INCOMPLETE };

enum HttpRequestHandler { CGI, STATIC, REDIRECT };

#endif