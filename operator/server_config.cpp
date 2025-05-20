

#include "server_config.h"

#include <android-base/strings.h>

using android::base::StartsWith;

namespace redroid {

namespace {
  constexpr auto kStunPrefix = "stun:";
}

ServerConfig::ServerConfig(const std::vector<std::string>& stuns)
    : stun_servers_(stuns) {}

Json::Value ServerConfig::ToJson() const {
  Json::Value ice_servers(Json::ValueType::arrayValue);
  for (const auto& str : stun_servers_) {
    Json::Value server;
    Json::Value urls(Json::ValueType::arrayValue);
    urls.append(StartsWith(str, kStunPrefix)? str: kStunPrefix + str);
    server["urls"] = urls;
    ice_servers.append(server);
  }
  Json::Value server_config;
  server_config["ice_servers"] = ice_servers;
  return server_config;
}

}  // namespace redroid
