#pragma once

#include "socket.h"
#include "thread_pool.h"
#include <thread>

namespace uring_http {

class HttpServer {
public:
  explicit HttpServer(size_t = std::thread::hardware_concurrency());

  void start(const char *port);

private:
  ThreadPool thread_pool_;
};

class HttpWorker {
public:
  explicit HttpWorker(const char *port);

private:
  Socket server_socket_{};
};

} // namespace uring_http