#pragma once

#include <file_descriptor.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <variant>

namespace uring_http {

class Socket : public FileDescriptor {
public:
  Socket();

  int get_file_descriptor();

  void bind(const char *port);

  void listen();
};

} // namespace uring_http
