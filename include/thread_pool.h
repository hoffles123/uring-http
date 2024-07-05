#pragma once

#include <functional>
#include <stop_token>
#include <thread>
#include <vector>

namespace uring_http {

class ThreadPool {
public:
  ThreadPool(size_t n, const std::function<void(std::stop_token)> &task);
  ~ThreadPool();

private:
  std::vector<std::jthread> workers_;
  std::stop_source stop_source_;
};

} // namespace uring_http
