#pragma once

#include <sys/types.h>
#include <array>
#include <cstdint>
#include <map>
#include <json/json.h>
#include <memory>
#include <optional>
#include <string>
#include <set>
#include <vector>


namespace Json {
class Value;
}

namespace redroid {

class RedroidConfig {
 public:
  static const RedroidConfig* Get();

  static std::unique_ptr<const RedroidConfig> GetFromFile(
      const std::string& path);
      
  RedroidConfig();
  RedroidConfig(RedroidConfig&&);
  ~RedroidConfig();
  RedroidConfig& operator=(RedroidConfig&&);

  struct DisplayConfig {
    int width;
    int height;
    int dpi;
    int refresh_rate_hz;
  };

  std::vector<DisplayConfig> display_configs() const;

 private:
  std::unique_ptr<Json::Value> dictionary_;

  void SetPath(const std::string& key, const std::string& path);
  bool LoadFromFile(const char* file);
  static RedroidConfig* BuildConfigImpl(const std::string& path);

  RedroidConfig(const RedroidConfig&) = delete;
  RedroidConfig& operator=(const RedroidConfig&) = delete;
};



}  // namespace redroid
