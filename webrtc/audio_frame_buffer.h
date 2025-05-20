#pragma once

#include <cinttypes>

namespace redroid {
namespace webrtc_streaming {

class AudioFrameBuffer {
 public:
  virtual ~AudioFrameBuffer() = default;

  virtual int bits_per_sample() const = 0;
  virtual int sample_rate() const = 0;
  virtual int channels() const = 0;
  virtual int frames() const = 0;
  virtual const uint8_t* data() const = 0;
};

}  // namespace webrtc_streaming
}  // namespace redroid
