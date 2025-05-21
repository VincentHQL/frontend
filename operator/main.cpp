

#include <map>
#include <string>

#include <android-base/logging.h>
#include <gflags/gflags.h>

#include "client_handler.h"
#include "device_handler.h"
#include "device_list_handler.h"
#include "websocket/websocket_handler.h"
#include "websocket/websocket_server.h"

DEFINE_int32(http_server_port, 8443, "The port for the http server.");
DEFINE_bool(use_secure_http, true, "Whether to use HTTPS or HTTP.");
DEFINE_string(assets_dir, "webrtc",
              "Directory with location of webpage assets.");
DEFINE_string(certs_dir, "webrtc/certs", "Directory to certificates.");
DEFINE_string(stun_server, "stun:stun.callwithus.com:3478",
              "host:port of STUN server to use for public address resolution");

namespace {

constexpr auto kRegisterDeviceUriPath = "/register_device";
constexpr auto kConnectClientUriPath = "/connect_client";
constexpr auto kListDevicesUriPath = "/list_devices";

}  // namespace

int main(int argc, char** argv) {
    ::android::base::InitLogging(argv, android::base::StderrLogger);
    // ::android::base::SetLogger(LogToStderrAndFiles({}, prefix));
//   redroid::DefaultSubprocessLogging(argv);
  ::gflags::ParseCommandLineFlags(&argc, &argv, true);

  redroid::DeviceRegistry device_registry;
  redroid::ServerConfig server_config({FLAGS_stun_server});

  redroid::WebSocketServer wss(
        "webrtc-operator", FLAGS_certs_dir, FLAGS_assets_dir, FLAGS_http_server_port);

  auto device_handler_factory_p =
      std::unique_ptr<redroid::WebSocketHandlerFactory>(
          new redroid::DeviceHandlerFactory(&device_registry, server_config));
  wss.RegisterHandlerFactory(kRegisterDeviceUriPath, std::move(device_handler_factory_p));
  auto client_handler_factory_p =
      std::unique_ptr<redroid::WebSocketHandlerFactory>(
          new redroid::ClientHandlerFactory(&device_registry, server_config));
  wss.RegisterHandlerFactory(kConnectClientUriPath, std::move(client_handler_factory_p));
  auto device_list_handler_factory_p =
      std::unique_ptr<redroid::WebSocketHandlerFactory>(
          new redroid::DeviceListHandlerFactory(device_registry));
  wss.RegisterHandlerFactory(kListDevicesUriPath, std::move(device_list_handler_factory_p));

  wss.Serve();
  return 0;
}
