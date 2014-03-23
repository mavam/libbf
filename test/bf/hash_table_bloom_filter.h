#ifndef BF_HASH_TABLE_BLOOM_FILTER_H
#define BF_HASH_TABLE_BLOOM_FILTER_H

#include <unordered_map>
#include <bf/bloom_filter.h>
#include <bf/hash.h>

/// A bloom filter that supplies exact counts.
class hash_table_bloom_filter : public bf::bloom_filter
{
public:
  /// Constructs an exact bloom filter.
  ///
  /// @param seed The seed for the hash function.
  hash_table_bloom_filter(size_t seed = 32);

  using bf::bloom_filter::add;
  using bf::bloom_filter::lookup;

  virtual void add(bf::object const& o) override;
  virtual size_t lookup(bf::object const& o) const override;
  virtual void clear() override;

private:
  bf::default_hash_function hash_function_;
  std::unordered_map<bf::digest, size_t> filter_;
};

#endif
