#include "io_uring.h"
#include <cstring>
#include <iostream>
#include <memory>
#include <thread>

namespace uring_http {

constexpr int IO_URING_QUEUE_SIZE = 2048;
constexpr int READ_SZ = 8192;

IOUring::IOUring() {
  if (const int result =
          io_uring_queue_init(IO_URING_QUEUE_SIZE, &io_uring_, 0);
      result != 0) {
    throw std::runtime_error("failed to invoke 'io_uring_queue_init'");
  }
}

IOUring::~IOUring() { io_uring_queue_exit(&io_uring_); }

IOUring &IOUring::get_instance() {
  thread_local IOUring instance;
  return instance;
}

void IOUring::event_loop(int server_socket_fd) {
  submit_accept_request(server_socket_fd);

  while (true) {
    io_uring_cqe *cqe;
    std::cout << std::this_thread::get_id() << " waitin on cqe" << std::endl;
    int result = io_uring_wait_cqe(&io_uring_, &cqe);
    std::cout << std::this_thread::get_id() << " finished waiting on cqe"
              << std::endl;
    if (result < 0) {
      throw std::runtime_error("failed to wait for io_uring cqe");
    }

    auto *raw_req = static_cast<URingRequest *>(io_uring_cqe_get_data(cqe));
    std::unique_ptr<URingRequest> req(raw_req);

    switch (req->event_type_) {
    case EVENT_TYPE_ACCEPT:
      std::cout << "received accept" << std::endl;
      submit_accept_request(server_socket_fd);
      submit_read_request(cqe->res);
      break;
    case EVENT_TYPE_READ:
      std::cout << "received read" << std::endl;
      handle_client_request(std::move(req));
      break;
    case EVENT_TYPE_WRITE:
      close(req->client_socket_fd_);
      std::cout << "received write" << std::endl;
      break;
    }
    std::cout << std::this_thread::get_id() << " check last cqe seen"
              << std::endl;
    io_uring_cqe_seen(&io_uring_, cqe);
    std::cout << std::this_thread::get_id() << " past last cqe seen"
              << std::endl;
  }
}

void IOUring::submit_accept_request(int server_socket_fd) {
  sockaddr client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  io_uring_sqe *sqe = io_uring_get_sqe(&io_uring_);
  io_uring_prep_accept(sqe, server_socket_fd, &client_addr, &client_addr_len,
                       0);
  auto req = std::make_unique<URingRequest>();
  req->event_type_ = EVENT_TYPE_ACCEPT;
  io_uring_sqe_set_data(sqe, req.get());
  io_uring_submit(&io_uring_);
  req.release();
}

void IOUring::submit_read_request(int client_socket_fd) {
  io_uring_sqe *sqe = io_uring_get_sqe(&io_uring_);
  auto req = std::make_unique<URingRequest>();
  req->event_type_ = EVENT_TYPE_READ;
  req->iov_.resize(1);
  req->iov_[0].iov_base = new char[READ_SZ];
  req->iov_[0].iov_len = READ_SZ;
  req->client_socket_fd_ = client_socket_fd;
  memset(req->iov_[0].iov_base, 0, READ_SZ);

  io_uring_prep_readv(sqe, client_socket_fd, &req->iov_[0], 1, 0);
  io_uring_sqe_set_data(sqe, req.get());
  io_uring_submit(&io_uring_);

  req.release();
}

void IOUring::handle_client_request(std::unique_ptr<URingRequest> req) {
  auto iov = req->iov_[0];
  std::cout << "Received data length: " << iov.iov_len << std::endl;
  std::cout << "Received data content: " << static_cast<char *>(iov.iov_base)
            << std::endl;
  submit_write_request(std::move(req));
}

void IOUring::submit_write_request(std::unique_ptr<URingRequest> req) {
  io_uring_sqe *sqe = io_uring_get_sqe(&io_uring_);
  req->event_type_ = EVENT_TYPE_WRITE;
  io_uring_prep_writev(sqe, req->client_socket_fd_, req->iov_.data(),
                       req->iov_.size(), 0);
  io_uring_sqe_set_data(sqe, req.get());
  io_uring_submit(&io_uring_);

  req.release();
}

} // namespace uring_http