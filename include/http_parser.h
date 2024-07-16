#pragma once

#include "http_msg.h"
#include <string>
#include <vector>

namespace uring_http {

class HttpParser {
public:
  HTTPRequest parse_request(std::string_view msg);

private:
  std::vector<std::string_view> splitByChar(std::string_view, char delim);

  std::vector<std::string_view> splitByStr(std::string_view,
                                           std::string_view delim);

  std::string_view trim_whitespace(std::string_view str);
};

} // namespace uring_http