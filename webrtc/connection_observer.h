

#pragma once

#include <functional>

#include <json/json.h>

namespace redroid {
namespace webrtc_streaming {

class ConnectionObserver {
 public:
  ConnectionObserver() = default;
  virtual ~ConnectionObserver() = default;

  virtual void OnConnected(
      std::function<void(const uint8_t*, size_t, bool)> ctrl_msg_sender) = 0;
  virtual void OnTouchEvent(const std::string& display_label, int x, int y,
                            bool down) = 0;
  virtual void OnMultiTouchEvent(const std::string& label, Json::Value id, Json::Value slot,
                                 Json::Value x, Json::Value y, bool down, int size) = 0;
  virtual void OnKeyboardEvent(uint16_t keycode, bool down) = 0;
  virtual void OnSwitchEvent(uint16_t code, bool state) = 0;
  virtual void OnAdbChannelOpen(
      std::function<bool(const uint8_t*, size_t)> adb_message_sender) = 0;
  virtual void OnAdbMessage(const uint8_t* msg, size_t size) = 0;
  virtual void OnControlChannelOpen(
      std::function<bool(const Json::Value)> control_message_sender) = 0;
  virtual void OnControlMessage(const uint8_t* msg, size_t size) = 0;
  virtual void OnBluetoothChannelOpen(
      std::function<bool(const uint8_t*, size_t)> bluetooth_message_sender) = 0;
  virtual void OnBluetoothMessage(const uint8_t* msg, size_t size) = 0;
  virtual void OnCameraData(const std::vector<char>& data) = 0;
};

class ConnectionObserverFactory {
 public:
  virtual ~ConnectionObserverFactory() = default;
  // Called when a new connection is requested
  virtual std::shared_ptr<ConnectionObserver> CreateObserver() = 0;
};

}  // namespace webrtc_streaming
}  // namespace redroid
