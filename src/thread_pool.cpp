#include <iostream>
#include <stop_token>

#include "thread_pool.h"

namespace uring_http {

ThreadPool::ThreadPool(size_t n) : size(n) {
  for (size_t i{0}; i < n; ++i) {
    workers_.emplace_back(
        [this](const std::stop_token &stop_token) { thread_loop(stop_token); });
  }
}

ThreadPool::~ThreadPool() {
  std::cout << "Cleaning up thread pool" << std::endl;
  stop_source_.request_stop();
  cv.notify_all();
}

size_t ThreadPool::get_size() const { return size; }

void ThreadPool::thread_loop(const std::stop_token &stop_token) {
  while (!stop_token.stop_requested()) {
    std::unique_lock lock(mut);
    cv.wait(lock, [this, stop_token] {
      return stop_token.stop_requested() || !tasks.empty();
    });
    if (stop_token.stop_requested())
      return;

    auto task = std::move(tasks.front());
    tasks.pop();
    lock.unlock();
    task(stop_token);
  }
}

} // namespace uring_http