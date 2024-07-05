#pragma once

#include <file_descriptor.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <variant>

namespace uring_http {

class Socket : FileDescriptor {
public:
  Socket();
  explicit Socket(int fd);

  void bind(const char *port);
  void listen();
};

} // namespace uring_http
