

#pragma once

#include <cinttypes>
#include <utility>

// This is not part of the webrtc api and therefore subject to change
#include <p2p/base/basic_packet_socket_factory.h>

namespace redroid {
namespace webrtc_streaming {

// rtc::BasicPacketSocketFactory is not part of the webrtc api so only functions
// from its upper class should be overridden here.
class PortRangeSocketFactory : public rtc::BasicPacketSocketFactory {
 public:
  PortRangeSocketFactory(rtc::Thread* thread,
                         std::pair<uint16_t, uint16_t> udp_port_range,
                         std::pair<uint16_t, uint16_t> tcp_port_range);

  rtc::AsyncPacketSocket* CreateUdpSocket(
      const rtc::SocketAddress& local_address, uint16_t min_port,
      uint16_t max_port) override;

  rtc::AsyncPacketSocket* CreateServerTcpSocket(
      const rtc::SocketAddress& local_address, uint16_t min_port,
      uint16_t max_port, int opts) override;

 private:
  std::pair<uint16_t, uint16_t> udp_port_range_;
  std::pair<uint16_t, uint16_t> tcp_port_range_;
};

}  // namespace webrtc_streaming
}  // namespace redroid
