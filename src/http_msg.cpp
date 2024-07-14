#include "http_msg.h"
#include <iostream>

namespace uring_http {

void HTTPRequest::print() {
  std::cout << "Method " << method << " Version " << version << " URL " << url
            << std::endl;
}

} // namespace uring_http