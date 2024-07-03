#include "file_descriptor.h"
#include <unistd.h>

namespace uring_http {

FileDescriptor::FileDescriptor() = default;

FileDescriptor::FileDescriptor(int fd) : fd_(fd) {}

FileDescriptor::~FileDescriptor() { close(fd_); }

} // namespace uring_http