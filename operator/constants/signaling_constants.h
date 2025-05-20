#pragma once

namespace redroid {
namespace webrtc_signaling {

constexpr auto kTypeField = "message_type";
constexpr auto kDeviceInfoField = "device_info";
constexpr auto kDeviceIdField = "device_id";
constexpr auto kClientIdField = "client_id";
constexpr auto kPayloadField = "payload";
constexpr auto kServersField = "ice_servers";
// These are defined in the IceServer dictionary
constexpr auto kUrlsField = "urls";
constexpr auto kUsernameField = "username";
constexpr auto kCredentialField = "credential";
constexpr auto kCredentialTypeField = "credentialType";

constexpr auto kRegisterType = "register";
constexpr auto kForwardType = "forward";
constexpr auto kConfigType = "config";
constexpr auto kConnectType = "connect";
constexpr auto kDeviceInfoType = "device_info";
constexpr auto kClientMessageType = "client_msg";
constexpr auto kClientDisconnectType = "client_disconnected";
constexpr auto kDeviceMessageType = "device_msg";

}  // namespace webrtc_signaling
}  // namespace redroid
