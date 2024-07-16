#include "http_server.h"
#include "io_uring.h"
#include <iostream>
#include <thread>

namespace uring_http {

HttpServer::HttpServer(size_t thread_count) : thread_pool_(thread_count) {}

HttpServer::~HttpServer() {
  std::cout << "Gracefully shutting down server..." << std::endl;
}

void HttpServer::start(const char *port) {
  std::cout << "Starting HTTP server on port " << port << "..." << std::endl;

  for (size_t i{0}; i < thread_pool_.get_size(); ++i) {
    auto worker = std::make_shared<HttpWorker>(port);
    thread_pool_.enqueue([worker] { worker->event_loop(); });
  }

  std::unique_lock lock(mut);
  cv.wait(lock);
}

HttpWorker::HttpWorker(const char *port) : server_socket_() {
  server_socket_.bind(port);
  server_socket_.listen();
}

void HttpWorker::event_loop() {
  try {
    IOUring::get_instance().event_loop(server_socket_.get_file_descriptor());
  } catch (const std::exception &e) {
    std::cerr << "Caught exception: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;
  }
}

} // namespace uring_http