

#pragma once

#include <map>
#include <optional>

#include <json/json.h>

namespace redroid {
namespace webrtc_streaming {

class ValidationResult {
 public:
  ValidationResult() = default;
  ValidationResult(const std::string &error) : error_(error) {}

  // Helper method to ensure a json object has the required fields convertible
  // to the appropriate types.
  static ValidationResult ValidateJsonObject(
      const Json::Value &obj, const std::string &type,
      const std::map<std::string, Json::ValueType> &required_fields,
      const std::map<std::string, Json::ValueType> &optional_fields = {});

  bool ok() const { return !error_.has_value(); }
  std::string error() const { return error_.value_or(""); }

 private:
  std::optional<std::string> error_;
};

}  // namespace webrtc_streaming
}  // namespace redroid
