#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include "cvd_video_frame_buffer.h"
#include "video_sink.h"

namespace redroid {

  struct WebRtcScProcessedFrame : public ScreenConnectorFrameInfo {
  // must support move semantic
  std::unique_ptr<CvdVideoFrameBuffer> buf_;
  std::unique_ptr<WebRtcScProcessedFrame> Clone() {
    // copy internal buffer, not move
    CvdVideoFrameBuffer* new_buffer = new CvdVideoFrameBuffer(*(buf_.get()));
    auto cloned_frame = std::make_unique<WebRtcScProcessedFrame>();
    cloned_frame->buf_ =
        std::move(std::unique_ptr<CvdVideoFrameBuffer>(new_buffer));
    return std::move(cloned_frame);
  }
};

class DisplayHandler {
 public:

  DisplayHandler(
      std::vector<std::shared_ptr<webrtc_streaming::VideoSink>> display_sinks,
      ScreenCapture& screen_capture);

  ~DisplayHandler() = default;

  [[noreturn]] void Loop();
  void SendLastFrame();

 private:
  GenerateProcessedFrameCallback GetScreenConnectorCallback();
  std::vector<std::shared_ptr<webrtc_streaming::VideoSink>> display_sinks_;
  ScreenCapture& screen_capture_;
  std::shared_ptr<webrtc_streaming::VideoFrameBuffer> last_buffer_;
  std::uint32_t last_buffer_display_ = 0;
  std::mutex last_buffer_mutex_;
  std::mutex next_frame_mutex_;
};
}  // namespace redroid
