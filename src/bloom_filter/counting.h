#ifndef BF_BLOOM_FILTER_BASIC_H
#define BF_BLOOM_FILTER_BASIC_H

#include "counter_vector.h"
#include "bloom_filter.h"
#include "hash.h"

namespace bf {

/// The counting Bloom filter.
class counting_bloom_filter : public bloom_filter
{
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

protected:
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
class spectral_rm_bloom_filter : public counting_bloom_filter
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
  hasher hasher2_;
  counter_vector cells2_;
};

} // namespace bf

#endif
