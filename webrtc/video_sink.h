

#pragma once

#include <memory>

#include "video_frame_buffer.h"

namespace redroid {
namespace webrtc_streaming {

class VideoSink {
 public:
  virtual ~VideoSink() = default;
  virtual void OnFrame(std::shared_ptr<VideoFrameBuffer> frame,
                       int64_t timestamp_us) = 0;
};

}  // namespace webrtc_streaming
}  // namespace redroid
