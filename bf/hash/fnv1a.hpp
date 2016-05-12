#ifndef BF_HASH_FNV1A_HPP
#define BF_HASH_FNV1A_HPP

#include <cstdint>

#include "bf/endian.hpp"

namespace bf {

class fnv1a {
public:
  static constexpr endian endian = endian::native;
  using result_type = size_t;

  void operator()(void const* data, size_t size) noexcept {
    auto p = static_cast<unsigned char const*>(data);
    auto e = p + size;
    while (p < e)
      state_ = (state_ ^ *p++) * 1099511628211u;
  }

  explicit operator size_t() noexcept {
    return state_;
  }

private:
  size_t state_ = 14695981039346656037u;
};

} // namespace bf

#endif // BF_HASH_H3_HPP
