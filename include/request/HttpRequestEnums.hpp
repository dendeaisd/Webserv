#pragma once
#ifndef HTTP_REQUEST_ENUMS_HPP
#define HTTP_REQUEST_ENUMS_HPP
enum class HttpRequestMethod {
  GET,
  POST,
  PUT,
  DELETE,
  HEAD,
  OPTIONS,
  PATCH,
  TRACE,
  CONNECT,
  UNKNOWN
};

enum class HttpRequestVersion { HTTP_1_0, HTTP_1_1, HTTP_2_0, UNKNOWN };

enum class HttpRequestParseStatus {
  NOT_PARSED,
  PARSED,
  INVALID,
  INCOMPLETE,
  EXPECT_CONTINUE,
  INPROGRESS
};

enum class HttpRequestHandler {
  CGI,
  STATIC,
  REDIRECT,
  FAVICON,
  DIRECTORY_LISTING,
  LIST_UPLOADS,
  SEND_UPLOADED_FILE,
  FILE_UPLOAD,
  BENCHMARK,
};

enum class HttpFileUploadStatus {
  NOT_STARTED,
  IN_PROGRESS,
  COMPLETE,
  FAILED,
};

#endif