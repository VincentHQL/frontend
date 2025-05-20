

#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <json/json.h>

#include <api/peer_connection_interface.h>
#include <pc/video_track_source.h>

#include "connection_observer.h"

namespace redroid {
namespace webrtc_streaming {

class InputChannelHandler;
class AdbChannelHandler;
class ControlChannelHandler;
class BluetoothChannelHandler;
class CameraChannelHandler;

class ClientVideoTrackInterface;
class ClientVideoTrackImpl;

class ClientHandler : public webrtc::PeerConnectionObserver,
                      public std::enable_shared_from_this<ClientHandler> {
 public:
  static std::shared_ptr<ClientHandler> Create(
      int client_id, std::shared_ptr<ConnectionObserver> observer,
      std::function<void(const Json::Value&)> send_client_cb,
      std::function<void(bool)> on_connection_changed_cb);
  ~ClientHandler() override;

  bool SetPeerConnection(
      rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection);

  bool AddDisplay(rtc::scoped_refptr<webrtc::VideoTrackInterface> track,
                  const std::string& label);

  bool AddAudio(rtc::scoped_refptr<webrtc::AudioTrackInterface> track,
                  const std::string& label);

  ClientVideoTrackInterface* GetCameraStream();

  void HandleMessage(const Json::Value& client_message);

  // CreateSessionDescriptionObserver implementation
  void OnCreateSDPSuccess(webrtc::SessionDescriptionInterface* desc);
  void OnCreateSDPFailure(webrtc::RTCError error);

  // SetSessionDescriptionObserver implementation
  void OnSetSDPFailure(webrtc::RTCError error);

  // PeerConnectionObserver implementation
  void OnSignalingChange(
      webrtc::PeerConnectionInterface::SignalingState new_state) override;
  void OnDataChannel(
      rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
  void OnRenegotiationNeeded() override;
  void OnStandardizedIceConnectionChange(
      webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
  void OnConnectionChange(
      webrtc::PeerConnectionInterface::PeerConnectionState new_state) override;
  void OnIceGatheringChange(
      webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
  void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
  // Gathering of an ICE candidate failed.
  // See https://w3c.github.io/webrtc-pc/#event-icecandidateerror
  // |host_candidate| is a stringified socket address.
  void OnIceCandidateError(const std::string& host_candidate,
                           const std::string& url, int error_code,
                           const std::string& error_text) override;
  // Gathering of an ICE candidate failed.
  // See https://w3c.github.io/webrtc-pc/#event-icecandidateerror
  void OnIceCandidateError(const std::string& address, int port,
                           const std::string& url, int error_code,
                           const std::string& error_text) override;
  void OnIceCandidatesRemoved(
      const std::vector<cricket::Candidate>& candidates) override;
  void OnTrack(
      rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override;
  void OnRemoveTrack(
      rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;

 private:
  enum class State {
      kNew,
      kCreatingOffer,
      kAwaitingAnswer,
      kConnecting,
      kConnected,
      kFailed,
  };
  ClientHandler(int client_id, std::shared_ptr<ConnectionObserver> observer,
                std::function<void(const Json::Value&)> send_client_cb,
                std::function<void(bool)> on_connection_changed_cb);

  // Intentionally private, disconnect the client by destroying the object.
  void Close();

  void LogAndReplyError(const std::string& error_msg) const;
  void AddPendingIceCandidates();

  int client_id_;
  State state_ = State::kNew;
  std::shared_ptr<ConnectionObserver> observer_;
  std::function<void(const Json::Value&)> send_to_client_;
  std::function<void(bool)> on_connection_changed_cb_;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
  std::vector<rtc::scoped_refptr<webrtc::DataChannelInterface>> data_channels_;
  std::unique_ptr<InputChannelHandler> input_handler_;
  std::unique_ptr<AdbChannelHandler> adb_handler_;
  std::unique_ptr<ControlChannelHandler> control_handler_;
  std::unique_ptr<BluetoothChannelHandler> bluetooth_handler_;
  std::unique_ptr<CameraChannelHandler> camera_data_handler_;
  std::unique_ptr<ClientVideoTrackImpl> camera_track_;
  bool remote_description_added_ = false;
  std::vector<std::unique_ptr<webrtc::IceCandidateInterface>>
      pending_ice_candidates_;
};

class ClientVideoTrackInterface {
 public:
  virtual ~ClientVideoTrackInterface() = default;
  virtual void AddOrUpdateSink(
      rtc::VideoSinkInterface<webrtc::VideoFrame>* sink,
      const rtc::VideoSinkWants& wants) = 0;
};

}  // namespace webrtc_streaming
}  // namespace redroid
