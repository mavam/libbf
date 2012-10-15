#ifndef BF_BLOOM_FILTER_STABLE_H
#define BF_BLOOM_FILTER_STABLE_H

#include <random>
#include "bloom_filter/basic.h"
#include "detail/evict.h"

namespace bf {

/// The stable Bloom filter.
template <typename Core = core<>, typename Generator = std::mt19937>
class stable : public basic<Core>
{
  typedef basic<Core> super;

public:
  typedef typename super::core_type core_type;

  /// Constructs a *stable Bloom filter*.
  /// @param generator The randomness generator.
  /// @param d The number of cells to decrement before adding an element.
  /// @param core The Bloom filter core.
  stable(size_t d, Core core)
    : super(std::move(core)), d_(d)
  {
    assert(d > 0);
  }

private:
  /// Adds an item to the stable Bloom filter. This invovles
  /// *(i)* decrementing *k* positions uniformly at random and *(ii)* setting
  /// the counter of the value *x* to its maximum value, i.e., all 1s.
  template <typename T>
  void add_impl(const T& x)
  {
    std::uniform_int_distribution<> unif(0, this->core_.store.size() - 1);
    for (size_t i = 0; i < d_; ++i)
      this->core_.store.decrement(unif(generator_));

    detail::basic::add(x, this->core_.store.max(), this->core_);
  }

  size_t d_;              ///< Number of cells to decrement before adding.
  Generator generator_;
};

} // namespace bf

#endif
