#ifndef BF_BLOOM_FILTER_BASIC_H
#define BF_BLOOM_FILTER_BASIC_H

#include <iosfwd>
#include <vector>
#include "bloom_filter.h"
#include "core.h"
#include "detail/basic.h"
#include "detail/spectral.h"

namespace bf {

/// The *basic Bloom filter*.
template <typename Core = core<>>
class basic : public bloom_filter<basic<Core>>
{
  typedef bloom_filter<basic<Core>> super;
  friend super;

public:
  typedef Core core_type;

  /// Computes @f$k^*@f$, the optimal number of hash functions for a given
  /// false positive rate @f$f@f$.
  /// @return The optimal number of hash functions.
  static double k(double f)
  {
    return std::floor(- (std::log(f) / std::log(2)));
  }

  /// Computes the capacity of a Bloom filter with respect to a given number
  /// of hash functions and number of cells in the store. The capacity is
  /// defined as the maximum number of items the Bloom filter can hold before
  /// the FP rate can no longer be guaranteed.
  /// @param k The number of hash functions.
  /// @param m The number of cells in the Bloom filter
  /// @return The maximum number of items the Bloom filter can hold.
  static double capacity(unsigned k, unsigned m)
  {
    return std::floor(m / k * std::log(2));
  }

  /// Constructs a *Basic Bloom Filter*.
  /// @param core The Bloom filter core.
  basic(Core core)
    : core_(std::move(core))
  {
  }

  /// Removes an item from the set.
  /// @tparam T The type of the item to delete.
  /// @param x An instance of type T.
  template <typename T>
  void remove(const T& x)
  {
    detail::basic::remove(x, core_);
  }

  /// Retrieves the number of hash functions.
  /// @return The number of hash functions.
  size_t k() const
  {
    return core_.hash.k();
  }

protected:
  template <typename T>
  void add_impl(const T& x)
  {
    detail::basic::add(x, core_);
  }

  template <typename T>
  size_t count_impl(const T& x) const
  {
    return detail::spectral::minimum(core_.positions(x), core_.store);
  }

  void clear_impl()
  {
    core_.store.reset();
  }

protected:
  core_type core_;

private:
  friend std::string to_string(basic const& bf)
  {
    return to_string(bf.core_);
  }
};

} // namespace bf

#endif
