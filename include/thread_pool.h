#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <stop_token>
#include <thread>
#include <vector>

namespace uring_http {

class ThreadPool {
public:
  ThreadPool(size_t n);

  ~ThreadPool();

  size_t get_size() { return size; };

  template <typename F, typename... Args> void enqueue(F &&f, Args &&...args);

private:
  size_t size;
  std::vector<std::jthread> workers_;
  std::queue<std::function<void(std::stop_token)>> tasks;

  std::stop_source stop_source_;
  std::mutex mut;
  std::condition_variable cv;
};

} // namespace uring_http
