#include "io_uring.h"
#include <cstring>
#include <filesystem>
#include <iostream>
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
    int result = io_uring_wait_cqe(&io_uring_, &cqe);
    if (result < 0) {
      throw std::runtime_error("failed to wait for io_uring cqe");
    }

    auto *raw_req = static_cast<URingRequest *>(io_uring_cqe_get_data(cqe));
    std::unique_ptr<URingRequest> req(raw_req);

    switch (req->event_type_) {
    case EVENT_TYPE_ACCEPT:
      submit_accept_request(server_socket_fd);
      submit_read_request(cqe->res);
      break;
    case EVENT_TYPE_READ:
      handle_client_request(std::move(req));
      break;
    case EVENT_TYPE_WRITE:
      close(req->client_socket_fd_);
      break;
    }
    io_uring_cqe_seen(&io_uring_, cqe);
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

  auto http_req = http_parser.parse_request(static_cast<char *>(iov.iov_base));

  auto writeReq = std::make_unique<URingRequest>();
  writeReq->client_socket_fd_ = req->client_socket_fd_;
  writeReq->iov_.resize(3);

  std::string s = "HTTP/1.0 200 OK\r\n";
  size_t s_len = s.size();
  writeReq->iov_[0].iov_base = new char[s_len];
  writeReq->iov_[0].iov_len = s_len;
  std::memcpy(writeReq->iov_[0].iov_base, s.c_str(), s_len);

  const char *home_dr = std::getenv("HOME");
  std::filesystem::path file_path =
      std::filesystem::path(home_dr) / http_req.url;

  if (std::filesystem::exists(file_path) &&
      std::filesystem::is_regular_file(file_path)) {
    const uintmax_t file_size = std::filesystem::file_size(file_path);

    std::ostringstream oss;
    oss << "content-length: " << file_size << "\r\n\r\n";
    std::string l = oss.str();
    writeReq->iov_[1].iov_base = new char[l.size()];
    writeReq->iov_[1].iov_len = l.size();
    std::memcpy(writeReq->iov_[1].iov_base, l.c_str(), l.size());

    char *buf = new char[file_size];
    int fd = open(file_path.c_str(), O_RDONLY);
    if (fd < 0)
      throw std::runtime_error("failed to open file");

    int ret = read(fd, buf, file_size);
    if (ret < file_size)
      throw std::runtime_error("encountered a short read");
    close(fd);
    writeReq->iov_[2].iov_base = buf;
    writeReq->iov_[2].iov_len = file_size;
  }

  submit_write_request(std::move(writeReq));
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