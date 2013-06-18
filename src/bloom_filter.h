#ifndef BF_BLOOM_FILTER_H
#define BF_BLOOM_FILTER_H

#include "hash.h"
#include "wrap.h"

namespace bf {

/// The abstract Bloom filter interface.
class bloom_filter
{
  bloom_filter(bloom_filter const&) = delete;
  bloom_filter& operator=(bloom_filter const&) = delete;

public:
  virtual ~bloom_filter() = default;

  /// Adds an element to the Bloom filter.
  /// @tparam T The type of the element to insert.
  /// @param x An instance of type `T`.
  template <typename T>
  void add(T const& x)
  {
    add_impl(hasher_(wrap(x)));
  }

  /// Retrieves the count of an element.
  /// @tparam T The type of the element to query.
  /// @param x An instance of type `T`.
  /// @return A frequency estimate for x.
  template <typename T>
  size_t lookup(T const& x) const
  {
    return lookup_impl(hasher_(wrap(x)));
  }

  /// Removes all items in the Bloom filter.
  virtual void clear() = 0;

protected:
  bloom_filter(hasher h)
    : hasher_(h)
  {
  };

  virtual void add_impl(std::vector<digest> const& digests) = 0;
  virtual size_t lookup_impl(std::vector<digest> const& digests) const = 0;

private:
  hasher hasher_;
};

} // namespace bf

#endif
