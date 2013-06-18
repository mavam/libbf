#ifndef BF_BLOOM_FILTER_BASIC_H
#define BF_BLOOM_FILTER_BASIC_H

#include "counter_vector.h"
#include "bloom_filter.h"

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

protected:
  virtual void add_impl(std::vector<digest> const& digests) override;
  virtual size_t lookup_impl(std::vector<digest> const& digests) const override;
  virtual void clear() override;

  counter_vector cells_;
};

/// A spectral Bloomfilter with minimum increase (MI) policy.
class spectral_mi_bloom_filter : public counting_bloom_filter
{
protected:
  virtual void add_impl(std::vector<digest> const& digests) override;
};

/// A spectral Bloomfilter with recurring minimum (RM) policy.
class spectral_rm_bloom_filter : public counting_bloom_filter
{
protected:
  virtual void add_impl(std::vector<digest> const& digests) override;
  virtual size_t lookup_impl(std::vector<digest> const& digests) const override;
  virtual void clear() override;

  /// Removes an element from the spectral Bloom filter.
  void remove(std::vector<digest> const& digests);

  counter_vector second_;
};

} // namespace bf

#endif
