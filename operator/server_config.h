

#pragma once

#include <string>
#include <vector>

#include <json/json.h>

namespace redroid {
class ServerConfig {
 public:
  ServerConfig(const std::vector<std::string>& stuns);

  Json::Value ToJson() const;

 private:
  std::vector<std::string> stun_servers_;
};
}  // namespace redroid
