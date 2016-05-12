#ifndef BF_BLOOM_FILTER_A2_HPP
#define BF_BLOOM_FILTER_A2_HPP

#include <bf/bloom_filter/basic.hpp>

namespace bf {

class a2_bloom_filter : public bloom_filter
{
public:
  /// Computes the optimal value number of hash functions based on a desired
  /// false-positive rate.
  ///
  /// @param fp The desired false-positive rate.
  ///
  /// @return The optimal number of hash functions for *fp*.
  static size_t k(double fp);

  /// @param fp The desired false-positive rate.
  ///
  /// @param cells The number of cells (bits) to use.
  ///
  /// @return The optimal capacity for *fp* and *m*.
  static size_t capacity(double fp, size_t cells);

  /// Constructs an @f$A^2$@f Bloom filter.
  ///
  /// @param k The number of hash functions to use in each Bloom filter.
  ///
  /// @param cells The number cells to use for both Bloom filters, i.e., each
  /// Bloom filter uses `cells / 2` cells.
  ///
  /// @param seed1 The initial seed for the first Bloom filter.
  ///
  /// @param seed2 The initial seed for the second Bloom filter.
  ///
  /// @pre `cells % 2 == 0`
  a2_bloom_filter(size_t k, size_t cells, size_t capacity,
                  size_t seed1 = 0, size_t seed2 = 0);

  using bloom_filter::add;
  using bloom_filter::lookup;

  virtual void add(object const& o) override;
  virtual size_t lookup(object const& o) const override;
  virtual void clear() override;

private:
  basic_bloom_filter first_;
  basic_bloom_filter second_;
  size_t items_ = 0; ///< Number of items in the active Bloom filter.
  size_t capacity_;  ///< Maximum number of items in the active Bloom filter.
};

} // namespace bf

#endif
