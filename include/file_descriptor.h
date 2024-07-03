#pragma once

namespace uring_http {

class FileDescriptor {
public:
  FileDescriptor();
  explicit FileDescriptor(int fd);
  ~FileDescriptor();

  // TODO move and copy?

private:
  int fd_{};
};

} // namespace uring_http
