#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include "cvd_video_frame_buffer.h"
#include "video_sink.h"

namespace redroid {

class DisplayHandler {
 public:

  DisplayHandler(
      std::vector<std::shared_ptr<webrtc_streaming::VideoSink>> display_sinks, ScreenCapture& screen_capture);

  ~DisplayHandler() = default;

  [[noreturn]] void Loop();
  void SendLastFrame();

 private:
  std::vector<std::shared_ptr<webrtc_streaming::VideoSink>> display_sinks_;
  ScreenCapture& screen_capture_;
  std::shared_ptr<webrtc_streaming::VideoFrameBuffer> last_buffer_;
  std::uint32_t last_buffer_display_ = 0;
  std::mutex last_buffer_mutex_;
  std::mutex next_frame_mutex_;
};
}  // namespace redroid
