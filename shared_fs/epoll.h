#pragma once

#include <sys/epoll.h>

#include <memory>
#include <optional>
#include <set>
#include <shared_mutex>

#include "shared_fd.h"
#include "result.h"

namespace redroid {

struct EpollEvent {
  SharedFD fd;
  uint32_t events;
};

class Epoll {
 public:
  static Result<Epoll> Create();
  Epoll(); // Invalid instance
  Epoll(Epoll&&);
  Epoll& operator=(Epoll&&);

  Result<void> Add(SharedFD fd, uint32_t events);
  Result<void> Modify(SharedFD fd, uint32_t events);
  Result<void> AddOrModify(SharedFD fd, uint32_t events);
  Result<void> Delete(SharedFD fd);
  Result<std::optional<EpollEvent>> Wait();

 private:
  Epoll(SharedFD);

  /**
   * This read-write mutex is read-locked to perform epoll operations, and
   * write-locked to replace the file descriptor.
   *
   * A read-write mutex is used here to make it possible to update the watched
   * set while the epoll resource is being waited on by another thread, while
   * excluding the possibility of the move constructor or assignment constructor
   * from stealing the file descriptor out from under waiting threads.
   */
  std::shared_mutex epoll_mutex_;
  SharedFD epoll_fd_;
  /**
   * This read-write mutex is read-locked when interacting with it as a const
   * std::set, and write-locked when interacting with it as a std::set.
   */
  std::shared_mutex watched_mutex_;
  std::set<SharedFD> watched_;
};

}  // namespace redroid
