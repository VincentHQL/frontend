

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <json/json.h>

#include "device_registry.h"
#include "server_config.h"
#include "signal_handler.h"
#include "websocket_handler.h"

namespace redroid {

class ClientHandler;

class DeviceHandler : public SignalHandler,
                      public std::enable_shared_from_this<DeviceHandler> {
 public:
  DeviceHandler(struct lws* wsi, DeviceRegistry* registry,
                const ServerConfig& server_config);

  Json::Value device_info() const { return device_info_; }

  size_t RegisterClient(std::shared_ptr<ClientHandler> client_handler);
  void SendClientMessage(size_t client_id, const Json::Value& message);
  void SendClientDisconnectMessage(size_t client_id);

  void OnClosed() override;

 protected:
  void handleMessage(const std::string& type,
                    const Json::Value& message) override;

 private:
  void HandleRegistrationRequest(const Json::Value& message);
  void HandleForward(const Json::Value& message);

  std::string device_id_;
  Json::Value device_info_;
  std::vector<std::weak_ptr<ClientHandler>> clients_;
};

class DeviceHandlerFactory : public WebSocketHandlerFactory {
 public:
  DeviceHandlerFactory(DeviceRegistry* registry,
                       const ServerConfig& server_config);
  std::shared_ptr<WebSocketHandler> Build(struct lws* wsi) override;

 private:
  DeviceRegistry* registry_;
  const ServerConfig& server_config_;
};
}  // namespace redroid
