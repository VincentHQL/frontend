

#include "vp8only_encoder_factory.h"

namespace redroid {
namespace webrtc_streaming {
VP8OnlyEncoderFactory::VP8OnlyEncoderFactory(
    std::unique_ptr<webrtc::VideoEncoderFactory> inner)
    : inner_(std::move(inner)) {}

std::vector<webrtc::SdpVideoFormat> VP8OnlyEncoderFactory::GetSupportedFormats()
    const {
  std::vector<webrtc::SdpVideoFormat> ret;
  // Allow only VP8
  for (auto& format : inner_->GetSupportedFormats()) {
    if (format.name == "VP8") {
      ret.push_back(format);
    }
  }
  return ret;
}

webrtc::VideoEncoderFactory::CodecInfo VP8OnlyEncoderFactory::QueryVideoEncoder(
    const webrtc::SdpVideoFormat& format) const {
  return inner_->QueryVideoEncoder(format);
}

std::unique_ptr<webrtc::VideoEncoder> VP8OnlyEncoderFactory::CreateVideoEncoder(
    const webrtc::SdpVideoFormat& format) {
  return inner_->CreateVideoEncoder(format);
}

std::unique_ptr<webrtc::VideoEncoderFactory::EncoderSelectorInterface>
VP8OnlyEncoderFactory::GetEncoderSelector() const {
  return inner_->GetEncoderSelector();
}

}  // namespace webrtc_streaming
}  // namespace redroid
