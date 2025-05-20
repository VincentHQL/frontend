

#pragma once

#include <cinttypes>

namespace redroid {
namespace webrtc_streaming {

class VideoFrameBuffer {
 public:
  virtual ~VideoFrameBuffer() = default;

  virtual int width() const = 0;
  virtual int height() const = 0;
  virtual int StrideY() const = 0;
  virtual int StrideU() const = 0;
  virtual int StrideV() const = 0;
  virtual const uint8_t* DataY() const = 0;
  virtual const uint8_t* DataU() const = 0;
  virtual const uint8_t* DataV() const = 0;
};

}  // namespace webrtc_streaming
}  // namespace redroid
