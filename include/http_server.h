#pragma once

#include "socket.h"
#include "thread_pool.h"
#include <condition_variable>
#include <mutex>
#include <thread>

namespace uring_http {

class HttpServer {
public:
  explicit HttpServer(size_t = std::thread::hardware_concurrency());

  ~HttpServer();

  void start(const char *port);

private:
  ThreadPool thread_pool_;
  std::mutex mut;
  std::condition_variable cv;
};

class HttpWorker {
public:
  explicit HttpWorker(const char *port);

  void event_loop();

private:
  Socket server_socket_{};
};

} // namespace uring_http