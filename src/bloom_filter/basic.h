#ifndef BF_BLOOM_FILTER_BASIC_H
#define BF_BLOOM_FILTER_BASIC_H

#include "bitvector.h"
#include "bloom_filter.h"

namespace bf {

/// The *basic Bloom filter*.
///
/// This Bloom filter does not use partitioning because it results in slightly
/// worse performance because partitioned Bloom filters tend to have more 1s
/// than non-partitioned filters.
class basic_bloom_filter : public bloom_filter
{
public:
  /// Computes the number of cells based on a false-positive rate and capacity.
  ///
  /// @param fp The desired false-positive rate
  ///
  /// @param capacity The maximum number of items.
  ///
  /// @return The number of cells to use that guarantee *fp* for *capacity*
  /// elements.
  static size_t m(double fp, size_t capacity);

  /// Computes @f$k^*$@f, the optimal number of hash functions for a given
  /// Bloom filter size (in terms of cells) and capacity.
  ///
  /// @param cells The number of cells in the Bloom filter (aka. *m*)
  ///
  /// @param capacity The maximum number of elements that can guarantee *fp*.
  ///
  /// @return The optimal number of hash functions for *cells* and *capacity*.
  static size_t k(size_t cells, size_t capacity);

  /// Constructs a basic Bloom filter.
  basic_bloom_filter(hash_policy const& hash, size_t cells);

protected:
  virtual void add_impl(std::vector<digest> const& digests) override;
  virtual size_t lookup_impl(std::vector<digest> const& digests) const override;
  virtual void clear() override;

private:
  bitvector bits_;
};

} // namespace bf

#endif
