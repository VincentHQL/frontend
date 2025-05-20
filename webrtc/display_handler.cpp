

#include "display_handler.h"

#include <chrono>
#include <functional>
#include <memory>

#include <libyuv.h>

namespace redroid {

DisplayHandler::DisplayHandler(
    std::vector<std::shared_ptr<webrtc_streaming::VideoSink>> display_sinks,
    ScreenCapture& screen_capture)
    : display_sinks_(display_sinks), screen_capture_(screen_capture) {
      // 注册回调
}

[[noreturn]] void DisplayHandler::Loop() {
  for (;;) {
    auto processed_frame = screen_connector_.OnNextFrame();
    // processed_frame has display number from the guest
    {
      std::lock_guard<std::mutex> lock(last_buffer_mutex_);
      std::shared_ptr<CvdVideoFrameBuffer> buffer = std::move(processed_frame.buf_);
      last_buffer_display_ = processed_frame.display_number_;
      last_buffer_ =
          std::static_pointer_cast<webrtc_streaming::VideoFrameBuffer>(buffer);
    }
    if (processed_frame.is_success_) {
      SendLastFrame();
    }
  }
}

void DisplayHandler::SendLastFrame() {
  std::shared_ptr<webrtc_streaming::VideoFrameBuffer> buffer;
  std::uint32_t buffer_display;
  {
    std::lock_guard<std::mutex> lock(last_buffer_mutex_);
    buffer = last_buffer_;
    buffer_display = last_buffer_display_;
  }
  if (!buffer) {
    // If a connection request arrives before the first frame is available don't
    // send any frame.
    return;
  }
  {
    // SendLastFrame can be called from multiple threads simultaneously, locking
    // here avoids injecting frames with the timestamps in the wrong order.
    std::lock_guard<std::mutex> lock(next_frame_mutex_);
    int64_t time_stamp =
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    display_sinks_[buffer_display]->OnFrame(buffer, time_stamp);
  }
}
}  // namespace redroid
