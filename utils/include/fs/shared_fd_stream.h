#ifndef REDROID_LIBS_FS_SHARED_FD_STREAM_H_
#define REDROID_LIBS_FS_SHARED_FD_STREAM_H_

#include <cstdio>
#include <istream>
#include <memory>
#include <ostream>
#include <streambuf>

#include "fs/shared_fd.h"

namespace redroid {

class SharedFDStreambuf : public std::streambuf {
 public:
  SharedFDStreambuf(SharedFD shared_fd);

 private:
  // Reading characters from the SharedFD.
  int underflow() override;
  std::streamsize xsgetn(char* dest, std::streamsize count) override;

  // Write characters to the SharedFD.
  int overflow(int c) override;
  std::streamsize xsputn(const char* source, std::streamsize count) override;

  int pbackfail(int c) override;

 private:
  SharedFD shared_fd_;

  static constexpr const ptrdiff_t kUngetSize = 128;
  static constexpr const ptrdiff_t kBufferSize = 4096 + kUngetSize;
  std::unique_ptr<char[]> read_buffer_ = nullptr;
};

class SharedFDIstream : public std::istream {
 public:
  SharedFDIstream(SharedFD shared_fd);

 private:
  SharedFDStreambuf buf_;
};

class SharedFDOstream : public std::ostream {
 public:
  SharedFDOstream(SharedFD shared_fd);

 private:
  SharedFDStreambuf buf_;
};

}  // namespace redroid

#endif