#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>

class Location {
 public:

   Location();

   std::string urlValue_;
   std::string rootValue_;
   std::string proxy_passValue_;
   std::string aliasValue_;
   std::string try_filesValue_;
   std::string indexValue_;
   std::string error_pageValue_;
   std::string rewriteValue_;
   std::string returnValue_;
   std::string access_logValue_;
   std::string denyValue_;
   std::string allowValue_;
   std::string add_headerValue_;
   std::string expiresValue_;
   std::string limit_reqValue_;
   std::string limit_connValue_;
   std::string proxy_set_headerValue_;
   std::string proxy_redirectValue_;
   std::string fastcgi_passValue_;
   std::string uwsgi_passValue_;
   std::string scgi_passValue_;
   std::string ContextsValue_;
   std::string ifValue_;
   std::string limit_exceptValue_;
};

#endif  // LOCATION_HPP
