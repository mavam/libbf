#ifndef BF_BLOOM_FILTER_BASIC_H
#define BF_BLOOM_FILTER_BASIC_H

#include <random>
#include "bitvector.h"
#include "bloom_filter.h"
#include "hash.h"

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
  /// @param k The number of hash functions to use.
  /// @param cells The number of cells in the bit vector.
  /// @param seed The initial seed used to construct the hash functions.
  template <
    typename HashFunction = default_hash_function,
    typename Hasher = default_hasher
  >
  basic_bloom_filter(size_t k, size_t cells, size_t seed = 0)
  {
    std::minstd_rand0 prng(seed);
    std::vector<hash_function> fns(k);
    for (size_t i = 0; i < k; ++i)
      fns[i] = HashFunction(prng());

    hasher_ = Hasher(std::move(fns));
    bits_.resize(cells);
  }

  /// Constructs a basic Bloom filter by given a desired false-positive
  /// probability and an expected number of elements. The implementation
  /// computes the optimal number of hash function and required space.
  /// @tparam HashFunction The hash function.
  /// @tparam HashFunction The hasher.
  /// @param fp The desired false-positive probability.
  /// @param capacity The desired false-positive probability.
  /// @param seed The initial seed used to construct the hash functions.
  template <
    typename HashFunction = default_hash_function,
    typename Hasher = default_hasher
  >
  basic_bloom_filter(double fp, size_t capacity, size_t seed = 0)
  {
    std::minstd_rand0 prng(seed);
    auto required_cells = m(fp, capacity);
    auto optimal_k = k(required_cells, 10);
    std::vector<hash_function> fns(optimal_k);
    for (size_t i = 0; i < optimal_k; ++i)
      fns[i] = HashFunction(prng());

    hasher_ = Hasher(std::move(fns));
    bits_.resize(required_cells);
  }

  using bloom_filter::add;
  using bloom_filter::lookup;

  virtual void add(object const& o) override;
  virtual size_t lookup(object const& o) const override;
  virtual void clear() override;

  /// Swaps two basic Bloom filters.
  void swap(basic_bloom_filter& other);

private:
  hasher hasher_;
  bitvector bits_;
};

} // namespace bf

#endif
