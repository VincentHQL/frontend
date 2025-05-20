

#include "device_list_handler.h"

namespace redroid {

DeviceListHandler::DeviceListHandler(struct lws* wsi,
                                     const DeviceRegistry& registry)
    : WebSocketHandler(wsi), registry_(registry) {}

void DeviceListHandler::OnReceive(const uint8_t* /*msg*/, size_t /*len*/,
                                  bool /*binary*/) {
  // Ignore the message, just send the reply
  Json::Value reply(Json::ValueType::arrayValue);

  for (const auto& id : registry_.ListDeviceIds()) {
    reply.append(id);
  }
  Json::StreamWriterBuilder json_factory;
  auto replyAsString = Json::writeString(json_factory, reply);
  EnqueueMessage(replyAsString.c_str(), replyAsString.size());
  Close();
}

void DeviceListHandler::OnConnected() {}

void DeviceListHandler::OnClosed() {}

DeviceListHandlerFactory::DeviceListHandlerFactory(const DeviceRegistry& registry)
  : registry_(registry) {}

std::shared_ptr<WebSocketHandler> DeviceListHandlerFactory::Build(struct lws* wsi) {
  return std::shared_ptr<WebSocketHandler>(new DeviceListHandler(wsi, registry_));
}
}  // namespace redroid
