#ifndef BF_BLOOM_FILTER_HPP
#define BF_BLOOM_FILTER_HPP

#include <bf/wrap.hpp>

namespace bf {

/// The abstract Bloom filter interface.
class bloom_filter
{
  bloom_filter(bloom_filter const&) = delete;
  bloom_filter& operator=(bloom_filter const&) = delete;

public:
  bloom_filter() = default;
  virtual ~bloom_filter() = default;

  /// Adds an element to the Bloom filter.
  /// @tparam T The type of the element to insert.
  /// @param x An instance of type `T`.
  template <typename T>
  void add(T const& x)
  {
    add(wrap(x));
  }

  /// Adds an element to the Bloom filter.
  /// @param o A wrapped object.
  virtual void add(object const& o) = 0;

  /// Retrieves the count of an element.
  /// @tparam T The type of the element to query.
  /// @param x An instance of type `T`.
  /// @return A frequency estimate for *x*.
  template <typename T>
  size_t lookup(T const& x) const
  {
    return lookup(wrap(x));
  }

  /// Retrieves the count of an element.
  /// @param o A wrapped object.
  /// @return A frequency estimate for *o*.
  virtual size_t lookup(object const& o) const = 0;

  /// Removes all items from the Bloom filter.
  virtual void clear() = 0;
};

} // namespace bf

#endif
