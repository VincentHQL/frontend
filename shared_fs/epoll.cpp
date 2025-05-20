#include "epoll.h"

#include <sys/epoll.h>

#include <memory>
#include <optional>
#include <set>
#include <shared_mutex>

#include "shared_fd.h"
#include "result.h"

namespace redroid {

Result<Epoll> Epoll::Create() {
  int fd = epoll_create1(EPOLL_CLOEXEC);
  if (fd == -1) {
    return RD_ERRNO("Failed to create epoll");
  }
  SharedFD shared{std::shared_ptr<FileInstance>(new FileInstance(fd, 0))};
  return Epoll(shared);
}

Epoll::Epoll() = default;

Epoll::Epoll(SharedFD epoll_fd) : epoll_fd_(epoll_fd) {}

Epoll::Epoll(Epoll&& other) {
  std::unique_lock own_watched(watched_mutex_, std::defer_lock);
  std::unique_lock own_epoll(epoll_mutex_, std::defer_lock);
  std::unique_lock other_epoll(other.epoll_mutex_, std::defer_lock);
  std::unique_lock other_watched(other.watched_mutex_, std::defer_lock);
  std::lock(own_watched, own_epoll, other_epoll, other_watched);

  epoll_fd_ = std::move(other.epoll_fd_);
  watched_ = std::move(other.watched_);
}

Epoll& Epoll::operator=(Epoll&& other) {
  std::unique_lock own_watched(watched_mutex_, std::defer_lock);
  std::unique_lock own_epoll(epoll_mutex_, std::defer_lock);
  std::unique_lock other_epoll(other.epoll_mutex_, std::defer_lock);
  std::unique_lock other_watched(other.watched_mutex_, std::defer_lock);
  std::lock(own_watched, own_epoll, other_epoll, other_watched);

  epoll_fd_ = std::move(other.epoll_fd_);
  watched_ = std::move(other.watched_);
  return *this;
}

Result<void> Epoll::Add(SharedFD fd, uint32_t events) {
  std::unique_lock watched_lock(watched_mutex_, std::defer_lock);
  std::shared_lock epoll_lock(epoll_mutex_, std::defer_lock);
  std::lock(watched_lock, epoll_lock);
  RD_EXPECT(epoll_fd_->IsOpen(), "Empty Epoll instance");

  if (watched_.count(fd) != 0) {
    return RD_ERRNO("Watched set already contains fd");
  }
  epoll_event event;
  event.events = events;
  event.data.fd = fd->fd_;
  int success = epoll_ctl(epoll_fd_->fd_, EPOLL_CTL_ADD, fd->fd_, &event);
  if (success != 0 && errno == EEXIST) {
    // We're already tracking this fd, don't drop it from the set.
    return RD_ERRNO("epoll_ctl: File descriptor was already present");
  } else if (success != 0) {
    return RD_ERRNO("epoll_ctl: Add failed");
  }
  watched_.insert(fd);
  return {};
}

Result<void> Epoll::AddOrModify(SharedFD fd, uint32_t events) {
  std::unique_lock watched_lock(watched_mutex_, std::defer_lock);
  std::shared_lock epoll_lock(epoll_mutex_, std::defer_lock);
  std::lock(watched_lock, epoll_lock);
  RD_EXPECT(epoll_fd_->IsOpen(), "Empty Epoll instance");

  epoll_event event;
  event.events = events;
  event.data.fd = fd->fd_;
  int operation = watched_.count(fd) == 0 ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
  int success = epoll_ctl(epoll_fd_->fd_, operation, fd->fd_, &event);
  if (success != 0) {
    std::string operation_str = operation == EPOLL_CTL_ADD ? "add" : "modify";
    return RD_ERRNO("epoll_ctl: Operation " << operation_str << " failed");
  }
  watched_.insert(fd);
  return {};
}

Result<void> Epoll::Modify(SharedFD fd, uint32_t events) {
  std::unique_lock watched_lock(watched_mutex_, std::defer_lock);
  std::shared_lock epoll_lock(epoll_mutex_, std::defer_lock);
  std::lock(watched_lock, epoll_lock);
  RD_EXPECT(epoll_fd_->IsOpen(), "Empty Epoll instance");

  if (watched_.count(fd) == 0) {
    return RD_ERR("Watched set did not contain fd");
  }
  epoll_event event;
  event.events = events;
  event.data.fd = fd->fd_;
  int success = epoll_ctl(epoll_fd_->fd_, EPOLL_CTL_MOD, fd->fd_, &event);
  if (success != 0) {
    return RD_ERRNO("epoll_ctl: Modify failed");
  }
  return {};
}

Result<void> Epoll::Delete(SharedFD fd) {
  std::unique_lock watched_lock(watched_mutex_, std::defer_lock);
  std::shared_lock epoll_lock(epoll_mutex_, std::defer_lock);
  std::lock(watched_lock, epoll_lock);
  RD_EXPECT(epoll_fd_->IsOpen(), "Empty Epoll instance");

  if (watched_.count(fd) == 0) {
    return RD_ERR("Watched set did not contain fd");
  }
  int success = epoll_ctl(epoll_fd_->fd_, EPOLL_CTL_DEL, fd->fd_, nullptr);
  if (success != 0) {
    return RD_ERRNO("epoll_ctl: Delete failed");
  }
  watched_.erase(fd);
  return {};
}

Result<std::optional<EpollEvent>> Epoll::Wait() {
  epoll_event event;
  int success;
  {
    std::shared_lock lock(epoll_mutex_);
    RD_EXPECT(epoll_fd_->IsOpen(), "Empty Epoll instance");
    success = epoll_wait(epoll_fd_->fd_, &event, 1, -1);
  }
  if (success == -1) {
    return RD_ERRNO("epoll_wait failed");
  } else if (success == 0) {
    return {};
  } else if (success != 1) {
    return RD_ERR("epoll_wait returned an unexpected value");
  }
  EpollEvent ret;
  ret.events = event.events;
  std::shared_lock lock(watched_mutex_);
  for (const auto& watched : watched_) {
    if (watched->fd_ == event.data.fd) {
      ret.fd = watched;
      break;
    }
  }
  if (!ret.fd->IsOpen()) {
    // Couldn't find the matching SharedFD to the file descriptor. We probably
    // lost the race to lock watched_mutex_ against a delete call. Treat this
    // as a spurious wakeup.
    return {};
  }
  return ret;
}

}  // namespace redroid
