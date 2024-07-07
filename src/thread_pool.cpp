#include <iostream>
#include <stop_token>

#include "thread_pool.h"

namespace uring_http {

ThreadPool::ThreadPool(size_t n) {
  size = n;
  for (size_t i{0}; i < n; ++i) {
    workers_.emplace_back([this](std::stop_token stop_token) {
      while (!stop_token.stop_requested()) {
        std::unique_lock lock(mut);
        cv.wait(lock, [this, &stop_token] {
          return stop_token.stop_requested() || !tasks.empty();
        });
        if (stop_token.stop_requested())
          return;

        auto task = std::move(tasks.front());
        tasks.pop();
        task(stop_token);
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  std::cout << "Cleaning up thread pool" << std::endl;
  stop_source_.request_stop();
  cv.notify_all();
}

template <typename F, typename... Args>
void ThreadPool::enqueue(F &&f, Args &&...args) {
  auto task = [f = std::forward<F>(f), ... args = std::forward<Args>(args)](
                  std::stop_token stop_token) {
    if (stop_token.stop_requested())
      return;
    f(args...);
  };

  tasks.emplace(std::move(task));
  cv.notify_one();
}

} // namespace uring_http