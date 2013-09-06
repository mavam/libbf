#include <test/bf/hash_table_bloom_filter.h>

#include <cmath>

namespace bf {

void hash_table_bloom_filter::add(object const& o)
{
  if (filter_.count(o) == 0)
  {
    filter_[o] = 0;
  }

  filter_[o]++;
}

size_t hash_table_bloom_filter::lookup(object const& o)
{
  return filter_[o];
}

void hash_table_bloom_filter::clear()
{
  filter_.clear();
}

} // namespace bf
