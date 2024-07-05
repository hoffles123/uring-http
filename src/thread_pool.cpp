#include <iostream>

#include "thread_pool.h"

namespace uring_http {

ThreadPool::ThreadPool(size_t n,
                       const std::function<void(std::stop_token)> &task) {
  for (size_t i = 0; i < n; i++) {
    workers_.emplace_back(task);
  }
}

ThreadPool::~ThreadPool() {
  std::cout << "Cleaning up thread pool" << std::endl;
  stop_source_.request_stop();
}

} // namespace uring_http