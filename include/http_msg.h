#pragma once

#include <string>
#include <vector>

namespace uring_http {

class HTTPRequest {
public:
  std::string method;
  std::string url;
  std::string version;
  std::vector<std::pair<std::string, std::string>> headers;

  void print();
};

} // namespace uring_http