

#include "video_track_source_impl.h"

#include <api/video/video_frame_buffer.h>

namespace redroid {
namespace webrtc_streaming {

namespace {

class VideoFrameWrapper : public webrtc::I420BufferInterface {
 public:
  VideoFrameWrapper(
      std::shared_ptr<::redroid::webrtc_streaming::VideoFrameBuffer>
          frame_buffer)
      : frame_buffer_(frame_buffer) {}
  ~VideoFrameWrapper() override = default;
  // From VideoFrameBuffer
  int width() const override { return frame_buffer_->width(); }
  int height() const override { return frame_buffer_->height(); }

  // From class PlanarYuvBuffer
  int StrideY() const override { return frame_buffer_->StrideY(); }
  int StrideU() const override { return frame_buffer_->StrideU(); }
  int StrideV() const override { return frame_buffer_->StrideV(); }

  // From class PlanarYuv8Buffer
  const uint8_t *DataY() const override { return frame_buffer_->DataY(); }
  const uint8_t *DataU() const override { return frame_buffer_->DataU(); }
  const uint8_t *DataV() const override { return frame_buffer_->DataV(); }

 private:
  std::shared_ptr<::redroid::webrtc_streaming::VideoFrameBuffer>
      frame_buffer_;
};

}  // namespace

VideoTrackSourceImpl::VideoTrackSourceImpl(int width, int height)
    : webrtc::VideoTrackSource(false), width_(width), height_(height) {}

void VideoTrackSourceImpl::OnFrame(std::shared_ptr<VideoFrameBuffer> frame,
                                   int64_t timestamp_us) {
  auto video_frame =
      webrtc::VideoFrame::Builder()
          .set_video_frame_buffer(
              new rtc::RefCountedObject<VideoFrameWrapper>(frame))
          .set_timestamp_us(timestamp_us)
          .build();
  broadcaster_.OnFrame(video_frame);
}

bool VideoTrackSourceImpl::GetStats(Stats *stats) {
  stats->input_height = height_;
  stats->input_width = width_;
  return true;
}

bool VideoTrackSourceImpl::SupportsEncodedOutput() const { return false; }
rtc::VideoSourceInterface<webrtc::VideoFrame> *VideoTrackSourceImpl::source() {
  return &broadcaster_;
}

}  // namespace webrtc_streaming
}  // namespace redroid
