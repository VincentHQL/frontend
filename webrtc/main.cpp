#include <linux/input.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <android-base/logging.h>
#include <android-base/strings.h>
#include <gflags/gflags.h>
#include <libyuv.h>

#include "shared_buf.h"
#include "shared_fd.h"
#include "files.h"
#include "audio_handler.h"
#include "client_server.h"
#include "connection_observer.h"
#include "display_handler.h"
#include "local_recorder.h"
#include "streamer.h"
#include "video_sink.h"
#include "server.h"
#include "redroid_config.h"
#include "logging.h"
#include "host_mode_ctrl.h"
#include "host_server.h"
#include "screen_connector.h"

using redroid::AudioHandler;
using redroid::CfConnectionObserverFactory;
using redroid::DisplayHandler;
using redroid::KernelLogEventsHandler;
using redroid::webrtc_streaming::LocalRecorder;
using redroid::webrtc_streaming::Streamer;
using redroid::webrtc_streaming::StreamerConfig;
using redroid::webrtc_streaming::VideoSink;
using redroid::webrtc_streaming::ServerConfig;

/**
  监听与Operator的连接
*/
class RedroidOperatorObserver
    : public redroid::webrtc_streaming::OperatorObserver {
 public:
  virtual ~RedroidOperatorObserver() = default;
  virtual void OnRegistered() override {
    LOG(VERBOSE) << "Registered with Operator";
  }
  virtual void OnClose() override {
    LOG(ERROR) << "Connection with Operator unexpectedly closed";
  }
  virtual void OnError() override {
    LOG(ERROR) << "Error encountered in connection with Operator";
  }
};

static std::vector<std::pair<std::string, std::string>> ParseHttpHeaders(
    const std::string& path) {
  auto fd = redroid::SharedFD::Open(path, O_RDONLY);
  if (!fd->IsOpen()) {
    LOG(WARNING) << "Unable to open operator (signaling server) headers file, "
                    "connecting to the operator will probably fail: "
                 << fd->StrError();
    return {};
  }
  std::string raw_headers;
  auto res = redroid::ReadAll(fd, &raw_headers);
  if (res < 0) {
    LOG(WARNING) << "Unable to open operator (signaling server) headers file, "
                    "connecting to the operator will probably fail: "
                 << fd->StrError();
    return {};
  }
  std::vector<std::pair<std::string, std::string>> headers;
  std::size_t raw_index = 0;
  while (raw_index < raw_headers.size()) {
    auto colon_pos = raw_headers.find(':', raw_index);
    if (colon_pos == std::string::npos) {
      LOG(ERROR)
          << "Expected to find ':' in each line of the operator headers file";
      break;
    }
    auto eol_pos = raw_headers.find('\n', colon_pos);
    if (eol_pos == std::string::npos) {
      eol_pos = raw_headers.size();
    }
    // If the file uses \r\n as line delimiters exclude the \r too.
    auto eov_pos = raw_headers[eol_pos - 1] == '\r'? eol_pos - 1: eol_pos;
    headers.emplace_back(
        raw_headers.substr(raw_index, colon_pos + 1 - raw_index),
        raw_headers.substr(colon_pos + 1, eov_pos - colon_pos - 1));
    raw_index = eol_pos + 1;
  }
  return headers;
}

std::unique_ptr<redroid::AudioServer> CreateAudioServer() {
  redroid::SharedFD audio_server_fd =
      redroid::SharedFD::Dup(FLAGS_audio_server_fd);
  close(FLAGS_audio_server_fd);
  return std::make_unique<redroid::AudioServer>(audio_server_fd);
}

fruit::Component<redroid::CustomActionConfigProvider> WebRtcComponent() {
  return fruit::createComponent()
      .install(redroid::ConfigFlagPlaceholder)
      .install(redroid::CustomActionsComponent);
};

int main(int argc, char** argv) {
  redroid::DefaultSubprocessLogging(argv);
  ::gflags::ParseCommandLineFlags(&argc, &argv, true);

  StreamerConfig streamer_config;

  // 读取配置文件
  auto cfg = Redroid::RedroidConfig::GetConfig();
  streamer_config.device_id = cfg->webrtc_device_id();
  streamer_config.client_files_port = cfg->port();
  streamer_config.tcp_port_range = cfg->webrtc_tcp_port_range();
  streamer_config.udp_port_range = cfg->webrtc_udp_port_range();
  streamer_config.operator_server.addr = cfg->sig_server_address();
  streamer_config.operator_server.port = cfg->sig_server_port();
  streamer_config.operator_server.path = cfg->sig_server_path();
  if (cfg->sig_server_secure()) {
    streamer_config.operator_server.security =
            cfg->sig_server_strict()
            ? ServerConfig::Security::kStrict
            : ServerConfig::Security::kAllowSelfSigned;
  } else {
    streamer_config.operator_server.security =
        ServerConfig::Security::kInsecure;
  }

  if (!cfg->sig_server_headers_path().empty()) {
    streamer_config.operator_server.http_headers =
        ParseHttpHeaders(cfg->sig_server_headers_path());
  }

  // 监听与客户端的连接
  auto observer_factory = std::make_shared<CfConnectionObserverFactory>();

  auto streamer = Streamer::Create(streamer_config, observer_factory);
  CHECK(streamer) << "Could not create streamer";

  uint32_t display_index = 0;
  std::vector<std::shared_ptr<VideoSink>> displays;
  // 获取屏幕设置， 可能存在多屏
  for (const auto& display_config : cfg->display_configs()) {
    const std::string display_id = "display_" + std::to_string(display_index);

    auto display =
        streamer->AddDisplay(display_id, display_config.width,
                             display_config.height, display_config.dpi, true);
    displays.push_back(display);

    ++display_index;
  }

  auto display_handler =
      std::make_shared<DisplayHandler>(std::move(displays), screen_connector);

  // 添加摄像头
  if (cfg->camera_server_port()) {
    auto camera_controller = streamer->AddCamera(instance.camera_server_port(),
                                                 instance.vsock_guest_cid());
    observer_factory->SetCameraHandler(camera_controller);
  }

  observer_factory->SetDisplayHandler(display_handler);

  // 音频录制
  std::shared_ptr<AudioHandler> audio_handler;
  if (cvd_config->enable_audio()) {
    auto audio_stream = streamer->AddAudioStream("audio");
    auto audio_server = CreateAudioServer();
    auto audio_source = streamer->GetAudioSource();
    audio_handler = std::make_shared<AudioHandler>(std::move(audio_server),
                                                   audio_stream, audio_source);
  }

  // 监听与 websocket服务端连接状态
  std::shared_ptr<redroid::webrtc_streaming::OperatorObserver> operator_observer(
      new CfOperatorObserver());
  streamer->Register(operator_observer);

  if (audio_handler) {
    // 启动音频接收录制
    audio_handler->Start();
  }
  // 主循环
  display_handler->Loop();

  return 0;
}
