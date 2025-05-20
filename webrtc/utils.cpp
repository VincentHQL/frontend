

#include "utils.h"

#include <map>

#include <json/json.h>

namespace redroid {
namespace webrtc_streaming {

namespace {

std::string ValidateField(const Json::Value &obj, const std::string &type,
                          const std::string &field_name,
                          const Json::ValueType &field_type, bool required) {
  if (!obj.isObject()) {
    return "Expected object with name-value pairs";
  }
  if (!obj.isMember(field_name) && !required) {
    return "";
  }
  if (!(obj.isMember(field_name) &&
        obj[field_name].isConvertibleTo(field_type))) {
    std::string error_msg = "Expected a field named '";
    error_msg += field_name + "' of type '";
    error_msg += std::to_string(field_type);
    error_msg += "'";
    if (!type.empty()) {
      error_msg += " in message of type '" + type + "'";
    }
    error_msg += ".";
    return error_msg;
  }
  return "";
}

}  // namespace

ValidationResult ValidationResult::ValidateJsonObject(
    const Json::Value &obj, const std::string &type,
    const std::map<std::string, Json::ValueType> &required_fields,
    const std::map<std::string, Json::ValueType> &optional_fields) {
  for (const auto &field_spec : required_fields) {
    auto result =
        ValidateField(obj, type, field_spec.first, field_spec.second, true);
    if (!result.empty()) {
      return {result};
    }
  }
  for (const auto &field_spec : optional_fields) {
    auto result =
        ValidateField(obj, type, field_spec.first, field_spec.second, false);
    if (!result.empty()) {
      return {result};
    }
  }
  return {};
}

}  // namespace webrtc_streaming
}  // namespace redroid
