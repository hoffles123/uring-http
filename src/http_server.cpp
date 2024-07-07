#include "http_server.h"
#include <iostream>
#include <thread>

namespace uring_http {

HttpServer::HttpServer(size_t threads) : thread_pool_(threads) {}

void HttpServer::start(const char *port) {
  std::cout << "Starting HTTP server on port " << port << "..." << std::endl;

  for (size_t i{0}; i < thread_pool_.get_size(); ++i) {
    HttpWorker worker{port};
  }
}

HttpWorker::HttpWorker(const char *port) : server_socket_() {
  server_socket_.bind(port);
  server_socket_.listen();
}

} // namespace uring_http