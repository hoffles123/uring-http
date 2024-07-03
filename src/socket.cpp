#include "socket.h"
#include <netdb.h>

namespace uring_http {

Socket::Socket() = default;

Socket::Socket(int fd) : FileDescriptor(fd) {}

void Socket::bind(int port) {
  addrinfo addr_hints{};
  addr_hints.ai_family = AF_UNSPEC;
  addr_hints.ai_socktype = SOCK_STREAM;
  addr_hints.ai_flags = AI_PASSIVE;
  addr_hints.ai_protocol = IPPROTO_TCP;
}

} // namespace uring_http
