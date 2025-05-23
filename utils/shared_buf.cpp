#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "fs/shared_buf.h"
#include "fs/shared_fd.h"

namespace redroid {

namespace {

const size_t BUFF_SIZE = 1 << 14;

} // namespace

ssize_t WriteAll(SharedFD fd, const char* buf, size_t size) {
  size_t total_written = 0;
  ssize_t written = 0;
  while ((written = fd->Write((void*)&(buf[total_written]), size - total_written)) > 0) {
    if (written < 0) {
      errno = fd->GetErrno();
      return written;
    }
    total_written += written;
    if (total_written == size) {
      break;
    }
  }
  return total_written;
}

ssize_t ReadExact(SharedFD fd, char* buf, size_t size) {
  size_t total_read = 0;
  ssize_t read = 0;
  while ((read = fd->Read((void*)&(buf[total_read]), size - total_read)) > 0) {
    if (read < 0) {
      errno = fd->GetErrno();
      return read;
    }
    total_read += read;
    if (total_read == size) {
      break;
    }
  }
  return total_read;
}

ssize_t ReadAll(SharedFD fd, std::string* buf) {
  char buff[BUFF_SIZE];
  std::stringstream ss;
  ssize_t read;
  while ((read = fd->Read(buff, BUFF_SIZE - 1)) > 0) {
    // this is necessary to avoid problems with having a '\0' in the middle of the buffer
    ss << std::string(buff, read);
  }
  if (read < 0) {
    errno = fd->GetErrno();
    return read;
  }
  *buf = ss.str();
  return buf->size();
}

ssize_t ReadExact(SharedFD fd, std::string* buf) {
  return ReadExact(fd, buf->data(), buf->size());
}

ssize_t ReadExact(SharedFD fd, std::vector<char>* buf) {
  return ReadExact(fd, buf->data(), buf->size());
}

ssize_t WriteAll(SharedFD fd, const std::string& buf) {
  return WriteAll(fd, buf.data(), buf.size());
}

ssize_t WriteAll(SharedFD fd, const std::vector<char>& buf) {
  return WriteAll(fd, buf.data(), buf.size());
}

bool SendAll(SharedFD sock, const std::string& msg) {
  ssize_t total_written{};
  if (!sock->IsOpen()) {
    return false;
  }
  while (total_written < static_cast<ssize_t>(msg.size())) {
    auto just_written = sock->Send(msg.c_str() + total_written,
                                   msg.size() - total_written, MSG_NOSIGNAL);
    if (just_written <= 0) {
      return false;
    }
    total_written += just_written;
  }
  return true;
}

std::string RecvAll(SharedFD sock, const size_t count) {
  size_t total_read{};
  if (!sock->IsOpen()) {
    return {};
  }
  std::unique_ptr<char[]> data(new char[count]);
  while (total_read < count) {
    auto just_read = sock->Read(data.get() + total_read, count - total_read);
    if (just_read <= 0) {
      return {};
    }
    total_read += just_read;
  }
  return {data.get(), count};
}

} // namespace redroid
