

#include "client_handler.h"

#include <android-base/logging.h>

#include "constants/signaling_constants.h"
#include "device_handler.h"

namespace redroid {

ClientHandler::ClientHandler(struct lws* wsi, DeviceRegistry* registry,
                             const ServerConfig& server_config)
    : SignalHandler(wsi, registry, server_config),
      device_handler_(),
      client_id_(0) {}

void ClientHandler::OnClosed() {
  auto device_handler = device_handler_.lock();
  if (device_handler) {
    device_handler->SendClientDisconnectMessage(client_id_);
  }
}

void ClientHandler::SendDeviceMessage(const Json::Value& device_message) {
  Json::Value message;
  message[webrtc_signaling::kTypeField] = webrtc_signaling::kDeviceMessageType;
  message[webrtc_signaling::kPayloadField] = device_message;
  Reply(message);
}

void ClientHandler::handleMessage(const std::string& type,
                                  const Json::Value& message) {
  LOG(INFO) << "ClientHandler::handleMessage: " << message.toStyledString();
  if (type == webrtc_signaling::kConnectType) {
    handleConnectionRequest(message);
  } else if (type == webrtc_signaling::kForwardType) {
    handleForward(message);
  } else {
    LogAndReplyError("Unknown message type: " + type);
  }
}

void ClientHandler::handleConnectionRequest(const Json::Value& message) {
  LOG(INFO) << "ClientHandler::handleConnectionRequest: " << message.toStyledString();
  if (client_id_ > 0) {
    LogAndReplyError(
        "Attempt to connect to multiple devices over same websocket");
    Close();
    return;
  }
  if (!message.isMember(webrtc_signaling::kDeviceIdField) ||
      !message[webrtc_signaling::kDeviceIdField].isString()) {
    LogAndReplyError("Invalid connection request: Missing device id");
    Close();
    return;
  }
  auto device_id = message[webrtc_signaling::kDeviceIdField].asString();
  // Always send the server config back, even if the requested device is not
  // registered. Applications may put clients on hold until the device is ready
  // to connect.
  SendServerConfig();

  auto device_handler = registry_->GetDevice(device_id);
  if (!device_handler) {
    LogAndReplyError("Connection failed: Device not found: '" + device_id +
                     "'");
    Close();
    return;
  }

  client_id_ = device_handler->RegisterClient(shared_from_this());
  device_handler_ = device_handler;
  Json::Value device_info_reply;
  device_info_reply[webrtc_signaling::kTypeField] =
      webrtc_signaling::kDeviceInfoType;
  device_info_reply[webrtc_signaling::kDeviceInfoField] =
      device_handler->device_info();
  Reply(device_info_reply);
}

void ClientHandler::handleForward(const Json::Value& message) {
  LOG(INFO) << "ClientHandler::handleForward: " << message.toStyledString();
  if (client_id_ == 0) {
    LogAndReplyError("Forward failed: No device asociated to client");
    Close();
    return;
  }
  if (!message.isMember(webrtc_signaling::kPayloadField)) {
    LogAndReplyError("Forward failed: No payload present in message");
    Close();
    return;
  }
  auto device_handler = device_handler_.lock();
  if (!device_handler) {
    LogAndReplyError("Forward failed: Device disconnected");
    // Disconnect this client since the device is gone
    Close();
    return;
  }
  device_handler->SendClientMessage(client_id_,
                                    message[webrtc_signaling::kPayloadField]);
}

ClientHandlerFactory::ClientHandlerFactory(DeviceRegistry* registry,
                                           const ServerConfig& server_config)
  : registry_(registry),
    server_config_(server_config) {}

std::shared_ptr<WebSocketHandler> ClientHandlerFactory::Build(struct lws* wsi) {
  return std::shared_ptr<WebSocketHandler>(
      new ClientHandler(wsi, registry_, server_config_));
}

}  // namespace redroid
