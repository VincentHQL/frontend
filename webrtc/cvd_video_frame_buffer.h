

#pragma once

#include <vector>

#include "video_frame_buffer.h"

namespace redroid {

class CvdVideoFrameBuffer : public webrtc_streaming::VideoFrameBuffer {
 public:
  CvdVideoFrameBuffer(int width, int height);
  CvdVideoFrameBuffer(CvdVideoFrameBuffer&& cvd_frame_buf) = default;
  CvdVideoFrameBuffer(const CvdVideoFrameBuffer& cvd_frame_buf) = default;
  CvdVideoFrameBuffer& operator=(CvdVideoFrameBuffer&& cvd_frame_buf) = default;
  CvdVideoFrameBuffer& operator=(const CvdVideoFrameBuffer& cvd_frame_buf) = default;
  CvdVideoFrameBuffer() = delete;

  ~CvdVideoFrameBuffer() override;

  int width() const override;
  int height() const override;

  int StrideY() const override;
  int StrideU() const override;
  int StrideV() const override;

  const uint8_t *DataY() const override;
  const uint8_t *DataU() const override;
  const uint8_t *DataV() const override;

  uint8_t *DataY() { return y_.data(); }
  uint8_t *DataU() { return u_.data(); }
  uint8_t *DataV() { return v_.data(); }

 private:
  const int width_;
  const int height_;
  std::vector<std::uint8_t> y_;
  std::vector<std::uint8_t> u_;
  std::vector<std::uint8_t> v_;
};

}
