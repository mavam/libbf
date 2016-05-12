#ifndef BF_BLOOM_FILTER_BITWISE_HPP
#define BF_BLOOM_FILTER_BITWISE_HPP

#include <bf/bloom_filter/basic.hpp>

namespace bf {

/// The bitwise Bloom filter.
class bitwise_bloom_filter : public bloom_filter
{
public:
  /// Constructs a bitwise Bloom filter.
  /// @param k The number of hash functions in the first level.
  /// @param cells0 The number of cells in the the first level.
  /// @param seed0 The seed for the first level.
  bitwise_bloom_filter(size_t k, size_t cells, size_t seed = 0);

  using bloom_filter::add;
  using bloom_filter::lookup;

  virtual void add(object const& o) override;
  virtual size_t lookup(object const& o) const override;
  virtual void clear() override;

private:
  /// Appends a new level.
  /// @post `levels_.size() += 1`
  void grow();

  size_t k_;
  size_t cells_;
  size_t seed_;
  std::vector<basic_bloom_filter> levels_;
};

} // namespace bf

#endif
