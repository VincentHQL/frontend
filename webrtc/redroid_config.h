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

class ConfigFragment {
public:
    virtual ~ConfigFragment();
  
    virtual std::string Name() const = 0;
    virtual Json::Value Serialize() const = 0;
    virtual bool Deserialize(const Json::Value&) = 0;
};

class RedroidConfig {
 public:
  static const RedroidConfig* Get();

  static std::unique_ptr<const RedroidConfig> GetFromFile(
      const std::string& path);
      
  static bool ConfigExists();

  RedroidConfig();
  RedroidConfig(RedroidConfig&&);
  ~RedroidConfig();
  RedroidConfig& operator=(RedroidConfig&&);

  bool LoadFragment(ConfigFragment&) const;

  struct DisplayConfig {
    int width;
    int height;
    int dpi;
    int refresh_rate_hz;
  };

  std::vector<DisplayConfig> display_configs() const;
  void set_display_configs(const std::vector<DisplayConfig>& display_configs);

 private:
  std::unique_ptr<Json::Value> dictionary_;

  void SetPath(const std::string& key, const std::string& path);
  bool LoadFromFile(const char* file);
  static RedroidConfig* BuildConfigImpl(const std::string& path);

  RedroidConfig(const RedroidConfig&) = delete;
  RedroidConfig& operator=(const RedroidConfig&) = delete;
};



}  // namespace redroid
