#ifndef BF_BLOOM_FILTER_BASIC_H
#define BF_BLOOM_FILTER_BASIC_H

#include "counter_vector.h"
#include "bloom_filter.h"
#include "hash.h"

namespace bf {

class a2_bloom_filter;
class spectral_mi_bloom_filter;
class spectral_rm_bloom_filter;

/// The counting Bloom filter.
class counting_bloom_filter : public bloom_filter
{
  friend a2_bloom_filter;
  friend spectral_mi_bloom_filter;
  friend spectral_rm_bloom_filter;

public:
  /// Constructs a basic Bloom filter.
  /// @param h The hasher.
  /// @param cells The number of cells.
  /// @param width The number of bits per cell.
  counting_bloom_filter(hasher h, size_t cells, size_t width);

  using bloom_filter::add;
  using bloom_filter::lookup;
  virtual void add(object const& o) override;
  virtual size_t lookup(object const& o) const override;
  virtual void clear() override;

private:
  /// Maps an object to the indices in the underlying counter vector.
  ///
  /// @param o The object to map.
  ///
  /// @param part If `false`, the function maps *o* uniformly into the counter
  /// vector modding each digest with the number of available cells. If
  /// `true`, the function partitions the counter vector into *k* distinct
  /// sub-vectors and mods each digest into one sub-vector.
  ///
  /// @return The indices corresponding to the digests of *o*.
  std::vector<size_t> find_indices(object const& o, bool part = false) const;

  /// Finds the minimum value in a list of arbitrary indices.
  /// @param indices The indices over which to compute the minimum.
  /// @return The minimum counter value over *indices*.
  size_t find_minimum(std::vector<size_t> const& indices) const;

  /// Finds one or more minimum indices for a list of arbitrary indices.
  /// @param indices The indices over which to compute the minimum.
  /// @return The indices corresponding to the minima in the counter vector.
  std::vector<size_t> find_minima(std::vector<size_t> const& indices) const;

  /// Increments a given set of indices in the underlying counter vector.
  /// @param indices The indices to increment.
  /// @return `true` iff no counter overflowed.
  bool increment(std::vector<size_t> const& indices, size_t value = 1);

  /// Decrements a given set of indices in the underlying counter vector.
  /// @param indices The indices to decrement.
  /// @return `true` iff no counter underflowed.
  bool decrement(std::vector<size_t> const& indices, size_t value = 1);

  /// Retrieves the counter for given cell index.
  /// @param index The index of the counter vector.
  /// @pre `index < cells.size()`
  size_t count(size_t index) const;

  hasher hasher_;
  counter_vector cells_;
};

/// A spectral Bloom filter with minimum increase (MI) policy.
class spectral_mi_bloom_filter : public counting_bloom_filter
{
public:
  /// Constructs a spectral MI Bloom filter.
  /// @param h The hasher.
  /// @param cells The number of cells.
  /// @param width The number of bits per cell.
  spectral_mi_bloom_filter(hasher h, size_t cells, size_t width);

  using bloom_filter::add;
  using bloom_filter::lookup;
  virtual void add(object const& o) override;
};

/// A spectral Bloom filter with recurring minimum (RM) policy.
class spectral_rm_bloom_filter : public bloom_filter
{
public:
  /// Constructs a spectral RM Bloom filter.
  /// @param h1 The first hasher.
  /// @param cells1 The number of cells in the first Bloom filter.
  /// @param width1 The number of bits per cell in the first Bloom filter.
  /// @param h2 The second hasher.
  /// @param cells2 The number of cells in the second Bloom filter.
  /// @param width2 The number of bits per cell in the second Bloom filter.
  spectral_rm_bloom_filter(hasher h1, size_t cells1, size_t width1,
                           hasher h2, size_t cells2, size_t width2);

  using bloom_filter::add;
  using bloom_filter::lookup;
  virtual void add(object const& o) override;
  virtual size_t lookup(object const& o) const override;
  virtual void clear() override;

  /// Removes an element from the spectral Bloom filter.
  void remove(object const& o);

private:
  counting_bloom_filter first_;
  counting_bloom_filter second_;
};

} // namespace bf

#endif
