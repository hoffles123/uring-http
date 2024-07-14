#include "http_parser.h"
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace uring_http {

HTTPRequest HttpParser::parse_request(std::string_view msg) {
  std::cout << msg << std::endl;

  const std::vector<std::string_view> request_line_list =
      splitByStr(msg, "\r\n");
  const std::vector<std::string_view> status_line_list =
      splitByChar(request_line_list[0], ' ');

  HTTPRequest req{};
  req.method = status_line_list[0];
  req.url = status_line_list[1];
  req.version = status_line_list[2];

  for (size_t i = 1; i < request_line_list.size(); ++i) {
    std::string_view header_line = request_line_list[i];
    std::vector<std::string_view> header = splitByChar(header_line, ':');
    if (header.size() == 2) {
      req.headers.emplace_back(header[0], trim_whitespace(header[1]));
    }
  }

  return req;
}

std::vector<std::string_view> HttpParser::splitByChar(std::string_view str,
                                                      const char delim) {
  std::vector<std::string_view> result;
  size_t segment_start = 0;
  size_t segment_end = 0;

  while ((segment_end = str.find(delim, segment_start)) != std::string::npos) {
    std::string_view token =
        str.substr(segment_start, segment_end - segment_start);
    result.emplace_back(token);
    segment_start = segment_end + 1;
  }

  result.emplace_back(str.substr(segment_start));
  return result;
}

std::vector<std::string_view> HttpParser::splitByStr(std::string_view str,
                                                     std::string_view delim) {
  std::vector<std::string_view> result;
  size_t segment_start = 0;
  size_t segment_end = 0;
  const size_t delimiter_length = delim.length();

  while ((segment_end = str.find(delim, segment_start)) != std::string::npos) {
    std::string_view token =
        str.substr(segment_start, segment_end - segment_start);
    result.emplace_back(token);
    segment_start = segment_end + delimiter_length;
  }

  result.emplace_back(str.substr(segment_start));
  return result;
}

std::string_view HttpParser::trim_whitespace(std::string_view str) {
  const auto first =
      std::find_if_not(str.cbegin(), str.cend(),
                       [](unsigned char c) { return std::isspace(c); });
  const auto last =
      std::find_if_not(str.crbegin(), str.crend(), [](unsigned char c) {
        return std::isspace(c);
      }).base();
  return (last <= first) ? std::string_view()
                         : std::string_view(
                               &*first, static_cast<std::size_t>(last - first));
}

} // namespace uring_http