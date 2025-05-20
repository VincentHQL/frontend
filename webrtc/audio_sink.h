

#pragma once

#include <memory>

#include "audio_frame_buffer.h"

namespace redroid {
namespace webrtc_streaming {

class AudioSink {
 public:
  virtual ~AudioSink() = default;
  virtual void OnFrame(std::shared_ptr<AudioFrameBuffer> frame,
                       int64_t timestamp_us) = 0;
};

}  // namespace webrtc_streaming
}  // namespace redroid
