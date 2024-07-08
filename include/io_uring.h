#pragma once

#include <liburing.h>
#include <memory>
#include <sys/uio.h>
#include <vector>

namespace uring_http {

enum EventType { EVENT_TYPE_ACCEPT, EVENT_TYPE_READ, EVENT_TYPE_WRITE };

struct URingRequest {
  EventType event_type_;
  int client_socket_fd_;
  std::vector<iovec> iov_;

  ~URingRequest() {
    for (auto &iov_elem : iov_) {
      if (iov_elem.iov_base != nullptr) {
        delete[] static_cast<char *>(iov_elem.iov_base);
      }
    }
  }
};

class IOUring {
public:
  IOUring();

  ~IOUring();

  static IOUring &get_instance();

  void event_loop(int server_socket_fd);

  void submit_accept_request(int server_socket_fd);

  void submit_read_request(int client_socket_fd);

  void submit_write_request(std::unique_ptr<URingRequest> req);

  void handle_client_request(std::unique_ptr<URingRequest> req);

private:
  io_uring io_uring_;
};

} // namespace uring_http
