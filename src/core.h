#ifndef BF_CORE_H
#define BF_CORE_H

#include "hash.h"
#include "partition.h"
#include "store.h"
#include "detail/core.h"

namespace bf {

/// The Bloom filter core class. It can be customized with different 
/// *store*, *hash*, and *partition* policies. The store policy determines the
/// underlying storage layout, the hash policy codifies the hash algorithm and
/// scheme, and the partition policy indicates how to split up the underlying
/// storage cells.
template <
    typename Store = fixed_width<unsigned char, std::allocator<unsigned char>>
  , typename Hash = default_hashing<>
  , typename Partition = no_partitioning
>
struct core
{
  typedef Store store_type;
  typedef Hash hash_type;
  typedef Partition part_type;
  typedef core<store_type, hash_type, part_type> core_type;

  /// Constructs a core object for fixed-width storage.
  /// @param cells The number of bits of the underlying bit vector.
  /// @param k The number of hash functions.
  /// @param width The number of bits per cell.
  /// @param parts The number of parts used in the partition policy.
  core(typename Store::size_type cells,
       unsigned k,
       unsigned width = 1,
       unsigned parts = 1)
    : store(cells, width), hash(k), part(parts)
  {
    assert(parts != 0);
    assert(cells % parts == 0);
  }

  void swap(core_type& c) // no throw
  {
    using std::swap;
    swap(store, c.store);
    swap(hash, c.hash);
    swap(part, c.part);
  }

  /// For a given item @f$x@f$, computes the (at most) @f$k@f$ corresponding
  /// cell indices in the store.
  /// @tparam T the type of the item.
  /// @tparam x The item to compute the positions for.
  /// @return A vector with positions in the store.
  template <typename T>
  std::vector<typename store_type::size_type> positions(T const& x) const
  {
    return detail::core::positions(x, *this);
  }

  friend std::string to_string(core_type const& c)
  {
    return to_string(c.store);
  }

  store_type store;
  hash_type hash;
  part_type part;
};

} // namespace bf

#endif
