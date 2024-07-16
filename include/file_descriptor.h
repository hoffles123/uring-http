#pragma once

namespace uring_http {

class FileDescriptor {
public:
  FileDescriptor();

  explicit FileDescriptor(int fd);

  ~FileDescriptor();

protected:
  int fd_{};
};

} // namespace uring_http
