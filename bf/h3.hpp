#ifndef BF_H3_HPP
#define BF_H3_HPP

#include <limits>
#include <random>

namespace bf {

/// An implementation of the H3 hash function family.
template <typename T, int N>
class h3
{
  static size_t const bits_per_byte =
    std::numeric_limits<unsigned char>::digits;

public:
  constexpr static size_t byte_range =
    std::numeric_limits<unsigned char>::max() + 1;

  h3(T seed = 0)
  {
    T bits[N * bits_per_byte];
    std::minstd_rand0 prng(seed);
    for (size_t bit = 0; bit < N * bits_per_byte; ++bit)
    {
      bits[bit] = 0;
      for (size_t i = 0; i < sizeof(T)/2; i++)
        bits[bit] = (bits[bit] << 16) | (prng() & 0xFFFF);
    }

    for (size_t byte = 0; byte < N; ++byte)
      for (size_t val = 0; val < byte_range; ++val)
      {
        bytes_[byte][val] = 0;
        for (size_t bit = 0; bit < bits_per_byte; ++bit)
          if (val & (1 << bit))
            bytes_[byte][val] ^= bits[byte * bits_per_byte + bit];
      }
  }

  T operator()(void const* data, size_t size, size_t offset = 0) const
  {
    auto *p = static_cast<unsigned char const*>(data);
    T result = 0;
    // Duff's Device.
    auto n = (size + 7) / 8;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
    switch (size % 8) 
    {
      case 0:	do { result ^= bytes_[offset++][*p++];
      case 7:      result ^= bytes_[offset++][*p++];
      case 6:      result ^= bytes_[offset++][*p++];
      case 5:      result ^= bytes_[offset++][*p++];
      case 4:      result ^= bytes_[offset++][*p++];
      case 3:      result ^= bytes_[offset++][*p++];
      case 2:      result ^= bytes_[offset++][*p++];
      case 1:      result ^= bytes_[offset++][*p++];
              } while ( --n > 0 );
    }
#pragma GCC diagnostic pop
    return result;
  }

private:
  T bytes_[N][byte_range];
};

} // namespace bf

#endif 
