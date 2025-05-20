

#pragma once

#include <stddef.h>

namespace redroid {

namespace webrtc_streaming {

// Interface to provide access to a stream originated on the client.
class AudioSource {
 public:
  // Returns the number of bytes read or a negative number in case of errors. If
  // muted is set to true, the contents of data should be considered to be all
  // 0s.
  virtual int GetMoreAudioData(void* data, int bytes_per_sample,
                               int samples_per_channel, int num_channels,
                               int sample_rate, bool& muted) = 0;

 protected:
  virtual ~AudioSource() = default;
};

}  // namespace webrtc_streaming
}  // namespace redroid
