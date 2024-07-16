#include "socket.h"
#include <cstring>
#include <netdb.h>
#include <stdexcept>

namespace uring_http {

Socket::Socket() = default;

int Socket::get_file_descriptor() { return fd_; }

void Socket::bind(const char *port) {
  addrinfo addr_hints;
  std::memset(&addr_hints, 0, sizeof(addr_hints));
  addr_hints.ai_family = AF_UNSPEC;
  addr_hints.ai_socktype = SOCK_STREAM;
  addr_hints.ai_flags = AI_PASSIVE;

  addrinfo *server_addr;
  if (getaddrinfo(nullptr, port, &addr_hints, &server_addr) == -1) {
    throw std::runtime_error("failed to get server address");
  }

  for (auto *node = server_addr; node != nullptr; node = node->ai_next) {
    fd_ = socket(node->ai_family, node->ai_socktype, node->ai_protocol);
    if (fd_ == -1) {
      throw std::runtime_error("failed to open a socket");
    }
    int option = 1;
    if (setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &option, sizeof(option)) ==
        -1) {
      throw std::runtime_error("failed to set socket options resue port");
    }
    if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) ==
        -1) {
      throw std::runtime_error("failed to set socket option reuse addr");
    }

    if (::bind(fd_, server_addr->ai_addr, server_addr->ai_addrlen) == -1) {
      throw std::runtime_error("failed to bind socket");
    }
    break;
  }

  freeaddrinfo(server_addr);
}

void Socket::listen() {
  if (::listen(fd_, SOMAXCONN) == -1) {
    throw std::runtime_error("failed to listen on socket");
  }
}

} // namespace uring_http
