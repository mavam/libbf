#include "bloom_filter/bitwise.h"

namespace bf {

bitwise_bloom_filter::bitwise_bloom_filter(size_t k, size_t cells, size_t seed)
  : k_(k), cells_(cells), seed_(seed)
{
  grow();
}

void bitwise_bloom_filter::add(object const& o)
{
  size_t i = 0;
  while (i != levels_.size())
  {
    if (! levels_[i].lookup(o))
      break;
    // FIXME: do not hash element more than once.
    levels_[i++].remove(o);
  }

  if (i == levels_.size())
    grow();

  levels_.back().add(o);
}

size_t bitwise_bloom_filter::lookup(object const& o) const
{
  size_t result = 0;
  for (size_t i = 0; i < levels_.size(); ++i)
    result += levels_[i].lookup(o) << i;
  return result;
}

void bitwise_bloom_filter::clear()
{
  levels_.clear();
  grow();
}

void bitwise_bloom_filter::grow()
{
  auto l = levels_.size();

  // TODO: come up with a reasonable growth scheme.
  static size_t const min_size = 1024;
  auto cells = cells_ / (2 * l);
  if (cells < min_size)
    cells = min_size;

  size_t seed = seed_;
  std::minstd_rand0 prng(seed);
  for (size_t i = 0; i < l; ++i)
    seed = prng();

  levels_.emplace_back(k_, cells, seed);
}

} // namespace bf
