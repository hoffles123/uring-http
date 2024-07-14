#include "file_descriptor.h"
#include <fcntl.h>
#include <filesystem>
#include <unistd.h>

namespace uring_http {

FileDescriptor::FileDescriptor() = default;

FileDescriptor::FileDescriptor(int fd) : fd_(fd) {}

FileDescriptor::~FileDescriptor() { close(fd_); }

FileDescriptor open_file(const std::filesystem::path &path) {
  int fd = open(path.c_str(), O_RDONLY);
  if (fd == -1) {
    throw std::runtime_error("failed to invoke 'open'");
  }
  return FileDescriptor{fd};
}

} // namespace uring_http