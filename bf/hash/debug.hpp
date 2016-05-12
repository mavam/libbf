#ifndef BF_HASH_DEBUG_HPP
#define BF_HASH_DEBUG_HPP

#include <cstddef>
#include <iomanip>
#include <iostream>
#include <vector>

#include "bf/endian.hpp"

namespace bf {

class debug {
public:
  static constexpr endian endian = endian::native;
  using result_type = size_t;

  void operator()(void const* data, size_t size) noexcept {
    auto p = static_cast<unsigned char const*>(data);
    for (auto i = 0u; i < size; ++i)
      buf_.push_back(p[i]);
  }

  explicit operator size_t() noexcept {
    using namespace std;
    cout << hex;
    cout << setfill('0');
    auto n = 0u;
    for (auto c : buf_) {
      cout << setw(2) << (unsigned)c << ' ';
      if (++n == 16) {
        cout << '\n';
        n = 0;
      }
    }
    cout << '\n';
    cout << dec;
    cout << setfill(' ');
    return buf_.size();
  }

private:
  std::vector<unsigned char> buf_;
};

} // namespace bf

#endif // BF_HASH_DEBUG_HPP
