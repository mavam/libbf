#ifndef BF_BLOOM_FILTER_A2_H
#define BF_BLOOM_FILTER_A2_H

#include "bloom_filter.h"
#include "core.h"
#include "detail/basic.h"
#include "detail/spectral.h"

namespace bf {

/// The @f$A^2@f$ Bloom filter.
template <typename Core = core<>>
class a2 : public bloom_filter<a2<Core>>
{
public:
  typedef Core core_type;

public:
  /// Compute @f$k^*@f$, the optimal number of hash functions for a given
  /// false positive rate @f$f@f$.
  /// @return The optimal number of hash functions.
  static double k(double f)
  {
    return std::floor(- (std::log(1 - std::sqrt(1 - f)) / std::log(2)));
  }

  /// Compute the capacity of a Bloom filter with respect to a given number
  /// of hash functions and number of cells in the store. The capacity is
  /// defined as the maximum number of items the Bloom filter can hold before
  /// the FP rate can no longer be guaranteed.
  /// @param k The number of hash functions.
  /// @param m The number of cells in the Bloom filter
  /// @return The maximum number of items the Bloom filter can hold.
  static double capacity(unsigned k, unsigned m)
  {
    return std::floor(m / (2 * k) * std::log(2));
  }

  /// Create an @f$A^2@f$ Bloom filter.
  /// @param core1 An rvalue reference to one core.
  /// @param capacity The capacity of a core. The default value of 0 results
  ///     in a capacity value that is derived from a false positive
  ///     probability of 1%.
  /// @todo Find a good minimum capacity value when the auto-calculation
  ///     computes a value that is close to 0.
  a2(core_type core, size_t capacity = 0)
    : items_(0), capacity_(capacity), core1_(std::move(core)), core2_(core1_)
  {
    if (core1_.store.size() != core2_.store.size())
      throw std::invalid_argument("different store sizes");

    if (core1_.store.width() != 1)
      throw std::invalid_argument("width of store 1 too large");

    if (core2_.store.width() != 1)
      throw std::invalid_argument("width of store 2 too large");

    if (! capacity_)
    {
      capacity_ = a2<Core>::capacity(core1_.hash.k(),
                                     core1_.store.size());
      if (! capacity_)
        capacity_ = 1;  // TODO: Find a good default value.
    }
  }

  template <typename T>
  void add(const T& x)
  {
    auto pos = core1_.positions(x);
    if (detail::spectral::minimum(pos, core1_.store))
      return;

    for (auto i : pos)
      core1_.store.increment(i);

    if (++items_ <= capacity_)
      return;

    core2_.store.reset();
    core1_.swap(core2_);
    for (auto i : pos)
      core1_.store.increment(i);

    items_ = 1;
  }

  template <typename T>
  void remove(const T& x) = delete;

  template <typename T>
  size_t count(const T& x) const
  {
    auto pos = core1_.positions(x);
    auto cnt = detail::spectral::minimum(pos, core1_.store);
    if (cnt)
      return cnt;
    return detail::spectral::minimum(core2_.positions(x), core2_.store);
  }

  /// Clear all bits.
  void clear()
  {
    core1_.store.reset();
    core2_.store.reset();
  }

private:
  friend std::string to_string(a2 const& bf)
  {
    auto str = to_string(bf.core1_);
    str += '\n';
    str += to_string(bf.core2_);
    return str;
  }

  size_t items_;     ///< Number of items in the first (active) core.
  size_t capacity_;  ///< Maximum number of items in the first core.
  core_type core1_;
  core_type core2_;
};

} // namespace bf

#endif
