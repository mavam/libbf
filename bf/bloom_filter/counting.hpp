#ifndef BF_BLOOM_FILTER_COUNTING_HPP
#define BF_BLOOM_FILTER_COUNTING_HPP

#include <bf/counter_vector.hpp>
#include <bf/bloom_filter.hpp>
#include <bf/hash.hpp>

namespace bf {

class spectral_mi_bloom_filter;
class spectral_rm_bloom_filter;

/// The counting Bloom filter.
class counting_bloom_filter : public bloom_filter
{
  friend spectral_mi_bloom_filter;
  friend spectral_rm_bloom_filter;

public:
  /// Constructs a counting Bloom filter.
  /// @param h The hasher.
  /// @param cells The number of cells.
  /// @param width The number of bits per cell.
  /// @param partition Whether to partition the bit vector per hash function.
  counting_bloom_filter(hasher h, size_t cells, size_t width,
                        bool partition = false);

  /// Move-constructs a counting Bloom filter.
  counting_bloom_filter(counting_bloom_filter&&) = default;

  using bloom_filter::add;
  using bloom_filter::lookup;

  virtual void add(object const& o) override;
  virtual size_t lookup(object const& o) const override;
  virtual void clear() override;

  /// Removes an element.
  /// @param o The object whose cells to decrement by 1.
  void remove(object const& o);

  template <typename T>
  void remove(T const& x)
  {
    remove(wrap(x));
  }

protected:
  /// Maps an object to the indices in the underlying counter vector.
  /// @param o The object to map.
  /// @return The indices corresponding to the digests of *o*.
  std::vector<size_t> find_indices(object const& o) const;

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
  bool partition_;
};

/// A spectral Bloom filter with minimum increase (MI) policy.
class spectral_mi_bloom_filter : public counting_bloom_filter
{
public:
  /// Constructs a spectral MI Bloom filter.
  /// @param h The hasher.
  /// @param cells The number of cells.
  /// @param width The number of bits per cell.
  /// @param partition Whether to partition the bit vector per hash function.
  spectral_mi_bloom_filter(hasher h, size_t cells, size_t width,
                           bool partition = false);

  using bloom_filter::add;
  using bloom_filter::lookup;
  using counting_bloom_filter::remove;
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
  /// @param partition Whether to partition the bit vector per hash function.
  spectral_rm_bloom_filter(hasher h1, size_t cells1, size_t width1,
                           hasher h2, size_t cells2, size_t width2,
                           bool partition = false);

  using bloom_filter::add;
  using bloom_filter::lookup;
  virtual void add(object const& o) override;
  virtual size_t lookup(object const& o) const override;
  virtual void clear() override;

  /// Removes an element.
  /// @param o The object whose cells to decrement by 1.
  void remove(object const& o);

private:
  counting_bloom_filter first_;
  counting_bloom_filter second_;
};

} // namespace bf

#endif
