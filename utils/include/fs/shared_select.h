#ifndef REDROID_LIBS_FS_SHARED_SELECT_H_
#define REDROID_LIBS_FS_SHARED_SELECT_H_

#include <set>

#include "shared_fd.h"

namespace redroid {
/**
 * The SharedFD version of fdset for the Select call.
 *
 * There are two types of methods. STL inspired methods and types use
 * all_lowercase_underscore notation.
 *
 * Methods that are inspired by POSIX Use UpperCamelCase.
 *
 * Assume that any mutation invalidates all iterators.
 */
class SharedFDSet {
 public:
  // These methods and types have more to do with the STL than POSIX,
  // so I'm using STL-compatible notation.
  typedef std::set<SharedFD>::iterator iterator;
  typedef std::set<SharedFD>::const_iterator const_iterator;

  iterator begin() { return value_.begin(); }
  iterator end() { return value_.end(); }
  const_iterator begin() const { return value_.begin(); }
  const_iterator end() const { return value_.end(); }

  void swap(SharedFDSet* rhs) {
    value_.swap(rhs->value_);
  }

  void Clr(const SharedFD& in) {
    value_.erase(in);
  }

  bool IsSet(const SharedFD& in) const {
    return value_.count(in) != 0;
  }

  void Set(const SharedFD& in) {
    value_.insert(in);
  }

  void Zero() {
    value_.clear();
  }

 private:
  std::set<SharedFD> value_;
};

/**
 * SharedFD version of select.
 *
 * read_set, write_set, and timeout are in/out parameters. This caller should keep
 * a copy of the original values if it wants to preserve them.
 */
int Select(SharedFDSet* read_set, SharedFDSet* write_set,
           SharedFDSet* error_set, struct timeval* timeout);

}  // namespace redroid

#endif  // REDROID_LIBS_FS_SHARED_SELECT_H_
