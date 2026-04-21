#pragma once
#include <cstdint>
#include <stdexcept>
#include <vector>


namespace Steam::Utils {
class GZip {
 public:
  static std::vector<uint8_t> gzip_decompress(const std::vector<uint8_t>& input,
                                              size_t expected_size);
};
}  // namespace Steam::Utils