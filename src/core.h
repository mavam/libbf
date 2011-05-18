#ifndef CORE_H
#define CORE_H

#include "hash.h"
#include "partition.h"
#include "store.h"
#include "detail/core.h"

namespace bf {

/// The Bloom filter core class. It can be customized with a different 
/// \e store, \e hash, and \e partition policy. The store policy determines the
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

    /// Create a core object for fixed-width storage.
    /// \param cells The number of bits of the underlying bit vector.
    /// \param k The number of hash functions.
    /// \param width The number of bits per cell.
    /// \param parts The number of parts used in the partition policy.
    core(typename Store::size_type cells, unsigned k, unsigned width = 1,
            unsigned parts = 1,
            typename std::enable_if<
                std::is_same<
                    store_type
                  , fixed_width<
                        typename store_type::block_type
                      , typename store_type::allocator_type
                    >
                >::value
            >* dummy = 0)
      : store(cells, width)
      , hash(k)
      , part(parts)
    {
        if (parts == 0)
            throw std::invalid_argument("zero parts");

        if (cells % parts)
            throw std::invalid_argument("parts do not divide cells");
    }

//    template <typename S, typename H, typename P>
    void swap(core_type& c) // no throw
    {
        std::swap(store, c.store);
        std::swap(hash, c.hash);
        std::swap(part, c.part);
    }

    /// For a given item \f$x\f$, compute the (at most) \f$k\f$ corresponding
    /// cell indices in the store.
    /// \tparam T the type of the item.
    /// \tparam x The item to compute the positions for.
    /// \return A vector with positions in the store.
    template <typename T>
    std::vector<typename store_type::size_type> positions(const T& x) const
    {
        return detail::core::positions(x, *this);
    }

    store_type store;
    hash_type hash;
    part_type part;
};

} // namespace bf

#endif
