#include <bf/bloom_filter/a2.hpp>

#include <cassert>

namespace bf {

size_t a2_bloom_filter::k(double fp) {
  return std::floor(-std::log(1 - std::sqrt(1 - fp)) / std::log(2));
}

size_t a2_bloom_filter::capacity(double fp, size_t cells) {
  return std::floor(cells / (2 * k(fp)) * std::log(2));
}

a2_bloom_filter::a2_bloom_filter(size_t k, size_t cells, size_t capacity,
                                 size_t seed1, size_t seed2)
    : first_(make_hasher(k, seed1), cells / 2),
      second_(make_hasher(k, seed2), cells / 2),
      capacity_(capacity) {
  assert(cells % 2 == 0);
}

void a2_bloom_filter::add(object const& o) {
  if (first_.lookup(o))
    return;
  first_.add(o); // FIXME: do not hash object twice for better performance.
  if (++items_ <= capacity_)
    return;
  items_ = 1;
  second_.clear();
  first_.swap(second_);
  first_.add(o);
}

size_t a2_bloom_filter::lookup(object const& o) const {
  auto r1 = first_.lookup(o);
  return r1 > 0 ? r1 : second_.lookup(o);
}

void a2_bloom_filter::clear() {
  first_.clear();
  second_.clear();
}

} // namespace bf
