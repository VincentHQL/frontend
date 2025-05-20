

#pragma once

#include <api/video_codecs/video_encoder_factory.h>
#include <api/video_codecs/video_encoder.h>

namespace redroid {
namespace webrtc_streaming {

class VP8OnlyEncoderFactory : public webrtc::VideoEncoderFactory {
 public:
  VP8OnlyEncoderFactory(std::unique_ptr<webrtc::VideoEncoderFactory> inner);

  std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override;

  CodecInfo QueryVideoEncoder(
      const webrtc::SdpVideoFormat& format) const override;

  std::unique_ptr<webrtc::VideoEncoder> CreateVideoEncoder(
      const webrtc::SdpVideoFormat& format) override;

  std::unique_ptr<EncoderSelectorInterface> GetEncoderSelector() const override;

 private:
  std::unique_ptr<webrtc::VideoEncoderFactory> inner_;
};

}  // namespace webrtc_streaming
}  // namespace redroid
