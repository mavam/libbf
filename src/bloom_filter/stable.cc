#include "bloom_filter/stable.h"

#include <cassert>

namespace bf {

stable_bloom_filter::stable_bloom_filter(hasher h, size_t cells, size_t width,
                                         size_t d)
  : counting_bloom_filter(std::move(h), cells, width),
    d_(d),
    unif_(0, cells - 1)
{
  assert(d <= cells);
}

void stable_bloom_filter::add(object const& o)
{
  for (size_t i = 0; i < d_; ++i)
    cells_.decrement(unif_(generator_));

  increment(find_indices(o), cells_.max());
}

} // namespace bf
