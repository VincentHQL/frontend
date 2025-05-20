

#pragma once

#include <memory>
#include <string>

#include <json/json.h>

#include "websocket_handler.h"
#include "device_registry.h"

namespace redroid {

class DeviceListHandler : public WebSocketHandler {
 public:
  DeviceListHandler(struct lws* wsi, const DeviceRegistry& registry);

  void OnReceive(const uint8_t* msg, size_t len, bool binary) override;
  void OnConnected() override;
  void OnClosed() override;

 private:
  const DeviceRegistry& registry_;
};

class DeviceListHandlerFactory : public WebSocketHandlerFactory {
 public:
  DeviceListHandlerFactory(const DeviceRegistry& registry);
  std::shared_ptr<WebSocketHandler> Build(struct lws* wsi) override;

 private:
  const DeviceRegistry& registry_;
};
}  // namespace redroid
