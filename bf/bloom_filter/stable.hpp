#ifndef BF_BLOOM_FILTER_STABLE_HPP
#define BF_BLOOM_FILTER_STABLE_HPP

#include <random>
#include <bf/bloom_filter/counting.hpp>

namespace bf {

/// A stable Bloom filter.
class stable_bloom_filter : public counting_bloom_filter
{
public:
  /// Constructs a stable Bloom filter.
  /// @param h The hasher.
  /// @param cells The number of cells.
  /// @param width The number of bits per cell.
  /// @param d The number of cells to decrement before adding an element.
  /// @pre `cells <= d`
  stable_bloom_filter(hasher h, size_t cells, size_t width, size_t d);

  /// Adds an item to the stable Bloom filter.
  /// This invovles first decrementing *k* positions uniformly at random and
  /// then setting the counter of *o* to all 1s.
  /// @param o The object to add.
  virtual void add(object const& o) override;

  using bloom_filter::add;
  using bloom_filter::lookup;

private:
  size_t d_;
  std::mt19937 generator_;
  std::uniform_int_distribution<> unif_;
};

} // namespace bf

#endif
