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

enum HttpRequestVersion {
 HTTP_1_0,
 HTTP_1_1,
 HTTP_2_0,
 UNKNOWN
};

enum HttpRequestParseStatus {
 NOT_PARSED,
 PARSED,
 INVALID,
 INCOMPLETE
};
