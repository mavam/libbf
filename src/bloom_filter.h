#ifndef BF_BLOOM_FILTER_H
#define BF_BLOOM_FILTER_H

#include <string>

namespace bf {

/// The Bloom filter interface.
/// \tparam Derived The type of the child class that implements a Bloom filter.
template <typename Derived>
class bloom_filter
{
public:

  /// Add an item to the set.
  /// \tparam T The type of the item to insert.
  /// \param x An instance of type T.
  template <typename T>
  void add(const T& x)
  {
    ++n_;
    derived().add(x);
  }

  /// Remove an item from the set.
  /// \tparam T The type of the item to delete.
  /// \param x An instance of type T.
  template <typename T>
  void remove(const T& x)
  {
    --n_;
    derived().remove(x);
  }

  /// Get the count of an item.
  /// \tparam T The type of the item to query.
  /// \param x An instance of type T.
  /// \return A frequency estimate for x.
  template <typename T>
  unsigned count(const T& x) const
  {
    return derived().count(x);
  }

  /// Remove all items from the set.
  void clear()
  {
    n_ = 0;
    derived().clear();
  }

  /// Get the number of items in the set, counted via add and remove.
  /// \return The number of items.
  unsigned long n() const
  {
    return n_;
  }

  /// Get a string representation of the Bloom filter.
  /// \return A string representing of the Bloom filter.
  std::string to_string() const
  {
    return derived().to_string();
  }

protected:
  bloom_filter() = default;

private:
  //
  // CRTP interface
  //
  template <typename Derived>
  Derived& derived() const
  {
    return *static_cast<Derived*>(this);
  }

  template <typename Derived>
  const Derived& derived() const
  {
    return *static_cast<const Derived*>(this);
  }

protected:
  /// The (multi)set cardinality.
  unsigned long n_;
};

} // namespace bf

#endif
