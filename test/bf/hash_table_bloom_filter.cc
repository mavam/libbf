#include "hash_table_bloom_filter.h"

hash_table_bloom_filter::hash_table_bloom_filter(size_t seed)
  : hash_function_(seed)
{
}

void hash_table_bloom_filter::add(bf::object const& o)
{
  filter_[hash_function_(o)]++;
}

size_t hash_table_bloom_filter::lookup(bf::object const& o) const
{
  auto i = filter_.find(hash_function_(o));
  if (i == filter_.end() )
    return i->second;
  return 0;
}

void hash_table_bloom_filter::clear()
{
  filter_.clear();
}
