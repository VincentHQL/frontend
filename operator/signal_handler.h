

#pragma once

#include <memory>
#include <string>

#include <json/json.h>

#include "device_registry.h"
#include "server_config.h"
#include "websocket/websocket_handler.h"

namespace redroid {

class SignalHandler : public WebSocketHandler {
 public:
  void OnReceive(const uint8_t* msg, size_t len, bool binary) override;
  void OnReceive(const uint8_t* msg, size_t len, bool binary,
                 bool is_final) override;
  void OnConnected() override;
 protected:
  SignalHandler(struct lws* wsi, DeviceRegistry* registry,
                const ServerConfig& server_config);

  virtual void handleMessage(const std::string& message_type,
                             const Json::Value& message) = 0;
  void SendServerConfig();

  void LogAndReplyError(const std::string& message);
  void Reply(const Json::Value& json);

  DeviceRegistry* registry_;
  const ServerConfig& server_config_;
  std::vector<uint8_t> receive_buffer_;
};
}  // namespace redroid
