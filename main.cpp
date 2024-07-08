#include "http_server.h"

int main() {
  uring_http::HttpServer server{};
  server.start("8000");
}