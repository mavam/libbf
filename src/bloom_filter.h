#ifndef BF_BLOOM_FILTER_H
#define BF_BLOOM_FILTER_H

#include <string>

namespace bf {

/// The Bloom filter interface.
/// @tparam Derived The type of the child class that implements a Bloom filter.
template <typename Derived>
class bloom_filter
{
public:

  /// Adds an item to the set.
  /// @tparam T The type of the item to insert.
  /// @param x An instance of type T.
  template <typename T>
  void add(T const& x)
  {
    ++n_;
    derived().add_impl(x);
  }

  /// Retrieves the count of an item.
  /// @tparam T The type of the item to query.
  /// @param x An instance of type T.
  /// @return A frequency estimate for x.
  template <typename T>
  size_t count(T const& x) const
  {
    return derived().count_impl(x);
  }

  /// Removes all items from the set.
  void clear()
  {
    n_ = 0;
    derived().clear_impl();
  }

  /// Retrieves the number of items in the set, counted via add() and remove().
  /// @return The number of items in the Bloom filter.
  size_t n() const
  {
    return n_;
  }

protected:
  bloom_filter() = default;

  size_t n_ = 0;

private:
  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  Derived const& derived() const
  {
    return static_cast<Derived const&>(*this);
  }
};

} // namespace bf

#endif
