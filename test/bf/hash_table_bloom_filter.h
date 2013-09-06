#ifndef BF_HASH_TABLE_BLOOM_FILTER_H
#define BF_HASH_TABLE_BLOOM_FILTER_H

#include <bf/bloom_filter.h>

namespace bf {

/// Bloom filter wrapper for an exact bloom filter.
class hash_table_bloom_filter : public bloom_filter
{
public:
  using bloom_filter::add;
  using bloom_filter::lookup;

  virtual void add(object const& o) override;
  virtual size_t lookup(object const& o) const override;
  virtual void clear() override;

private:
  std::unordered_map<object const& o, size_t> filter_;
};

} // namespace bf

#endif
