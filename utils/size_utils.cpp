#include "size_utils.h"

#include <unistd.h>

namespace redroid {

uint64_t AlignToPowerOf2(uint64_t val, uint8_t align_log) {
  uint64_t align = 1ULL << align_log;
  return ((val + (align - 1)) / align) * align;
}

}  // namespace redroid
