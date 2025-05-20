#pragma once

#include <inttypes.h>

#include <android-base/endian.h>

// The utilities in android-base/endian.h still require the use of regular int
// types to store values with any endianness, which requires the user to
// remember to manually do the required conversions, which is prone to errors.
// The types introduced here allow handling these values safely.

namespace redroid {

#define DECLARE_TYPE(new_type, base_type, to_new, to_base)                \
  class new_type {                                                        \
   public:                                                                \
    new_type() = default;                                                 \
    explicit new_type(base_type val) : inner_(to_new(val)) {}             \
    new_type(const new_type&) = default;                                  \
    new_type& operator=(const new_type& other) = default;                 \
    volatile new_type& operator=(const new_type& other) volatile {        \
      inner_ = other.inner_;                                              \
      return *this;                                                       \
    }                                                                     \
    base_type as_##base_type() const volatile { return to_base(inner_); } \
                                                                          \
   private:                                                               \
    base_type inner_;                                                     \
  };                                                                      \
  static_assert(sizeof(new_type) == sizeof(base_type))

DECLARE_TYPE(Le16, uint16_t, htole16, le16toh);
DECLARE_TYPE(Le32, uint32_t, htole32, le32toh);
DECLARE_TYPE(Le64, uint64_t, htole64, le64toh);
DECLARE_TYPE(Be16, uint16_t, htobe16, be16toh);
DECLARE_TYPE(Be32, uint32_t, htobe32, be32toh);
DECLARE_TYPE(Be64, uint64_t, htobe64, be64toh);

#undef DECLARE_TYPE

}  // namespace redroid