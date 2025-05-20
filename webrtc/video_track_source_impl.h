

#pragma once

#include <media/base/video_broadcaster.h>
#include <pc/video_track_source.h>

#include "video_sink.h"

namespace redroid {
namespace webrtc_streaming {

class VideoTrackSourceImpl : public webrtc::VideoTrackSource {
 public:
  VideoTrackSourceImpl(int width, int height);

  void OnFrame(std::shared_ptr<VideoFrameBuffer> frame, int64_t timestamp_us);

  // Returns false if no stats are available, e.g, for a remote source, or a
  // source which has not seen its first frame yet.
  //
  // Implementation should avoid blocking.
  bool GetStats(Stats* stats) override;

  bool SupportsEncodedOutput() const override;
  void GenerateKeyFrame() override {}
  void AddEncodedSink(
      rtc::VideoSinkInterface<webrtc::RecordableEncodedFrame>* sink) override {}
  void RemoveEncodedSink(
      rtc::VideoSinkInterface<webrtc::RecordableEncodedFrame>* sink) override {}

  rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override;

 private:
  int width_;
  int height_;
  rtc::VideoBroadcaster broadcaster_;
};

// Wraps a VideoTrackSourceImpl as an implementation of the VideoSink interface.
// This is needed as the VideoTrackSourceImpl is a reference counted object that
// should only be referenced by rtc::scoped_refptr pointers, but the
// VideoSink interface is not a reference counted object and therefore not
// compatible with that kind of pointers. This class can be referenced by a
// shared pointer and it in turn holds a scoped_refptr to the wrapped object.
class VideoTrackSourceImplSinkWrapper : public VideoSink {
 public:
  virtual ~VideoTrackSourceImplSinkWrapper() = default;

  VideoTrackSourceImplSinkWrapper(rtc::scoped_refptr<VideoTrackSourceImpl> obj)
      : track_source_impl_(obj) {}

  void OnFrame(std::shared_ptr<VideoFrameBuffer> frame,
               int64_t timestamp_us) override {
    track_source_impl_->OnFrame(frame, timestamp_us);
  }

 private:
  rtc::scoped_refptr<VideoTrackSourceImpl> track_source_impl_;
};

}  // namespace webrtc_streaming
}  // namespace redroid
