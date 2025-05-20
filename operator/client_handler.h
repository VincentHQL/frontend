

#pragma once

#include <memory>
#include <string>

#include <json/json.h>

#include "device_registry.h"
#include "server_config.h"
#include "signal_handler.h"
#include "websocket_handler.h"

namespace redroid {
class DeviceHandler;
class ClientHandler : public SignalHandler,
                      public std::enable_shared_from_this<ClientHandler> {
 public:
  ClientHandler(struct lws* wsi, DeviceRegistry* registry,
                const ServerConfig& server_config);
  void SendDeviceMessage(const Json::Value& message);

  void OnClosed() override;

 protected:
  void handleMessage(const std::string& type,
                    const Json::Value& message) override;

 private:
  void handleConnectionRequest(const Json::Value& message);
  void handleForward(const Json::Value& message);

  std::weak_ptr<DeviceHandler> device_handler_;
  // The device handler assigns this to each client to be able to differentiate
  // them.
  size_t client_id_;
};

class ClientHandlerFactory : public WebSocketHandlerFactory {
 public:
  ClientHandlerFactory(DeviceRegistry* registry,
                       const ServerConfig& server_config);
  std::shared_ptr<WebSocketHandler> Build(struct lws* wsi) override;

 private:
  DeviceRegistry* registry_;
  const ServerConfig& server_config_;
};
}  // namespace redroid
