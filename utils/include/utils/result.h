#pragma once

#include <optional>
#include <type_traits>

#include <android-base/logging.h>
#include <android-base/result.h>  // IWYU pragma: export

namespace redroid {

using android::base::Result;

#define RD_ERR_MSG()                             \
  "  at " << __FILE__ << ":" << __LINE__ << "\n" \
          << "  in " << __PRETTY_FUNCTION__

/**
 * Error return macro that includes the location in the file in the error
 * message. Use RD_ERRNO when including information from errno, otherwise use
 * the base RD_ERR macro.
 *
 * Example usage:
 *
 *     if (mkdir(path.c_str()) != 0) {
 *       return RD_ERRNO("mkdir(\"" << path << "\") failed: "
 *                       << strerror(errno));
 *     }
 *
 * This will return an error with the text
 *
 *     mkdir(...) failed: ...
 *       at path/to/file.cpp:50
 *       in Result<std::string> MyFunction()
 */
#define RD_ERR(MSG) android::base::Error() << MSG << "\n" << RD_ERR_MSG()
#define RD_ERRNO(MSG)                        \
  android::base::ErrnoError() << MSG << "\n" \
                              << RD_ERR_MSG() << "\n  with errno " << errno

template <typename T>
T OutcomeDereference(std::optional<T>&& value) {
  return std::move(*value);
}

template <typename T>
typename std::conditional_t<std::is_void_v<T>, bool, T> OutcomeDereference(
    Result<T>&& result) {
  if constexpr (std::is_void<T>::value) {
    return result.ok();
  } else {
    return std::move(*result);
  }
}

template <typename T>
typename std::enable_if<std::is_convertible_v<T, bool>, T>::type
OutcomeDereference(T&& value) {
  return std::forward<T>(value);
}

inline bool TypeIsSuccess(bool value) { return value; }

template <typename T>
bool TypeIsSuccess(std::optional<T>& value) {
  return value.has_value();
}

template <typename T>
bool TypeIsSuccess(Result<T>& value) {
  return value.ok();
}

template <typename T>
bool TypeIsSuccess(Result<T>&& value) {
  return value.ok();
}

inline auto ErrorFromType(bool) {
  return (android::base::Error() << "Received `false`").str();
}

template <typename T>
inline auto ErrorFromType(std::optional<T>) {
  return (android::base::Error() << "Received empty optional").str();
}

template <typename T>
auto ErrorFromType(Result<T>& value) {
  return value.error();
}

template <typename T>
auto ErrorFromType(Result<T>&& value) {
  return value.error();
}

#define RD_EXPECT_OVERLOAD(_1, _2, NAME, ...) NAME

#define RD_EXPECT2(RESULT, MSG)                                  \
  ({                                                             \
    decltype(RESULT)&& macro_intermediate_result = RESULT;       \
    if (!TypeIsSuccess(macro_intermediate_result)) {             \
      return android::base::Error()                              \
             << ErrorFromType(macro_intermediate_result) << "\n" \
             << MSG << "\n"                                      \
             << RD_ERR_MSG() << "\n"                             \
             << "  for RD_EXPECT(" << #RESULT << ")";            \
    };                                                           \
    OutcomeDereference(std::move(macro_intermediate_result));    \
  })

#define RD_EXPECT1(RESULT) RD_EXPECT2(RESULT, "Received error")

/**
 * Error propagation macro that can be used as an expression.
 *
 * The first argument can be either a Result or a type that is convertible to
 * a boolean. A successful result will return the value inside the result, or
 * a conversion to a `true` value will return the unconverted value. This is
 * useful for e.g. pointers which can be tested through boolean conversion.
 *
 * In the failure case, this macro will return from the containing function
 * with a failing Result. The failing result will include information about the
 * call site, details from the optional second argument if given, and details
 * from the failing inner expression if it is a Result.
 *
 * This macro must be invoked only in functions that return a Result.
 *
 * Example usage:
 *
 *     Result<std::string> CreateTempDir();
 *
 *     Result<std::string> CreatePopulatedTempDir() {
 *       std::string dir = RD_EXPECT(CreateTempDir(), "Failed to create dir");
 *       // Do something with dir
 *       return dir;
 *     }
 *
 * If CreateTempDir fails, the function will returna Result with an error
 * message that looks like
 *
 *      Internal error
 *        at /path/to/otherfile.cpp:50
 *        in Result<std::string> CreateTempDir()
 *      Failed to create dir
 *        at /path/to/file.cpp:81:
 *        in Result<std::string> CreatePopulatedTempDir()
 *        for RD_EXPECT(CreateTempDir())
 */
#define RD_EXPECT(...) \
  RD_EXPECT_OVERLOAD(__VA_ARGS__, RD_EXPECT2, RD_EXPECT1)(__VA_ARGS__)

}  // namespace cuttlefish
