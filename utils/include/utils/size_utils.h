#pragma once

#include <stdint.h>

namespace redroid {

// Keep the full disk size a multiple of 64k, for crosvm's virtio_blk driver
constexpr int DISK_SIZE_SHIFT = 16;

// Keep all partitions 4k aligned, for host performance reasons
constexpr int PARTITION_SIZE_SHIFT = 12;

// Returns the smallest multiple of 2^align_log greater than or equal to val.
uint64_t AlignToPowerOf2(uint64_t val, uint8_t align_log);

}  // namespace redroid
