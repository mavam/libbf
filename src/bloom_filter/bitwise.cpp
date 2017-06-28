#include <bf/bloom_filter/bitwise.hpp>

namespace bf {

bitwise_bloom_filter::bitwise_bloom_filter(size_t k, size_t cells, size_t seed)
    : k_(k), cells_(cells), seed_(seed) {
  grow();
}

void bitwise_bloom_filter::add(object const& o) {
  size_t l = 0;
  // FIXME: do not hash element more than once for better performance.
  while (l < levels_.size())
    if (levels_[l].lookup(o)) {
      levels_[l++].remove(o);
    } else {
      levels_[l].add(o);
      return;
    }

  grow();
  levels_.back().add(o);
}

size_t bitwise_bloom_filter::lookup(object const& o) const {
  size_t result = 0;
  for (size_t l = 0; l < levels_.size(); ++l)
    result += levels_[l].lookup(o) << l;
  return result;
}

void bitwise_bloom_filter::clear() {
  levels_.clear();
  grow();
}

void bitwise_bloom_filter::grow() {
  auto l = levels_.size();

  // TODO: come up with a reasonable growth scheme.
  static size_t const min_size = 128;
  auto cells = l == 0 ? min_size : cells_ / (2 * l);
  if (cells < min_size)
    cells = min_size;

  size_t seed = seed_;
  std::minstd_rand0 prng(seed);
  for (size_t i = 0; i < l; ++i)
    seed = prng();

  levels_.emplace_back(make_hasher(k_, seed), cells);
}

} // namespace bf
