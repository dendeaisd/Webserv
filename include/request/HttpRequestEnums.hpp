// i would change the enums wth an enum class,
// to avoid accidental collisions(since the enum vals are scoped within the
// enums name) like redefiniton of the 'UNKNOWN' but i'm not sure if it s even
// necessary given the size of our project

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
  UNKNOWN
};

enum HttpRequestVersion { HTTP_1_0, HTTP_1_1, HTTP_2_0, UNKNOWN };

enum HttpRequestParseStatus { NOT_PARSED, PARSED, INVALID, INCOMPLETE };
