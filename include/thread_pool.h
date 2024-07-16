#pragma once

#include <condition_variable>
#include <functional>
#include <iostream>
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

  size_t get_size() const;

  void thread_loop(const std::stop_token &stop_token);

  template <typename F, typename... Args>
  inline void enqueue(F &&f, Args &&...args) {
    auto task = [f = std::forward<F>(f), ... args = std::forward<Args>(args)](
                    const std::stop_token &stop_token) {
      if (stop_token.stop_requested())
        return;
      f(args...);
    };

    tasks.emplace(std::move(task));
    cv.notify_all();
  }

private:
  size_t size;
  std::vector<std::jthread> workers_;
  std::queue<std::function<void(std::stop_token)>> tasks;

  std::mutex mut;
  std::condition_variable cv;
};

} // namespace uring_http
