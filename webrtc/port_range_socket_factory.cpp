

#include "port_range_socket_factory.h"

#include <android-base/logging.h>

namespace redroid {
namespace webrtc_streaming {

namespace {

std::pair<uint16_t, uint16_t> IntersectPortRanges(
    std::pair<uint16_t, uint16_t> own_range, uint16_t min_port,
    uint16_t max_port) {
  if (own_range.first == own_range.second && own_range.first == 0) {
    // No range configured
    return {min_port, max_port};
  }
  if (min_port == max_port && max_port == 0) {
    // No range requested, use configured
    return own_range;
  }
  uint16_t own_min_port = own_range.first;
  uint16_t own_max_port = own_range.second;

  if (min_port > own_max_port || max_port < own_min_port) {
    // Ranges don't intersect
    LOG(WARNING) << "Port ranges don't intersect: requested=[" << min_port
                 << "," << max_port << "], configured=[" << own_min_port << ","
                 << own_max_port << "]";
  }
  return {std::max(min_port, own_min_port), std::min(max_port, own_max_port)};
}

}  // namespace

PortRangeSocketFactory::PortRangeSocketFactory(
    rtc::Thread* thread, std::pair<uint16_t, uint16_t> udp_port_range,
    std::pair<uint16_t, uint16_t> tcp_port_range)
    : rtc::BasicPacketSocketFactory(thread),
      udp_port_range_(udp_port_range),
      tcp_port_range_(tcp_port_range) {}

rtc::AsyncPacketSocket* PortRangeSocketFactory::CreateUdpSocket(
    const rtc::SocketAddress& local_address, uint16_t min_port,
    uint16_t max_port) {
  auto port_range = IntersectPortRanges(udp_port_range_, min_port, max_port);
  if (port_range.second < port_range.first) {
    // Own range doesn't intersect with requested range
    return nullptr;
  }
  return rtc::BasicPacketSocketFactory::CreateUdpSocket(
      local_address, port_range.first, port_range.second);
}

rtc::AsyncPacketSocket* PortRangeSocketFactory::CreateServerTcpSocket(
    const rtc::SocketAddress& local_address, uint16_t min_port,
    uint16_t max_port, int opts) {
  auto port_range = IntersectPortRanges(tcp_port_range_, min_port, max_port);
  if (port_range.second < port_range.first) {
    // Own range doesn't intersect with requested range
    return nullptr;
  }

  return rtc::BasicPacketSocketFactory::CreateServerTcpSocket(
      local_address, port_range.first, port_range.second, opts);
}

}  // namespace webrtc_streaming
}  // namespace redroid
