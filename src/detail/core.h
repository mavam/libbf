#ifndef DETAIL_CORE_H
#define DETAIL_CORE_H

namespace bf {
namespace detail {
namespace core {

/// For a given item \f$x\f$, compute \f$k\f$ unique indices to be used in
/// the store.
/// \tparam T the type of the item.
/// \tparam x The item to compute the positions for.
/// \return A vector with positions for the store.
template <typename T, typename Core>
typename std::enable_if<
    std::is_same<typename Core::part_type, partitioning>::value
  , std::vector<typename Core::store_type::size_type>
>::type
positions(const T& x, const Core& core)
{
    std::vector<typename Core::store_type::size_type> v;
    v.reserve(core.hash.k());

    auto h = core.hash.hash(x);
    for (unsigned i = 0; i < h.size(); ++i)
        v.push_back(core.part.position(h[i], i, core.store.size()));

    v.shrink_to_fit();
    return v;
}

/// For a given item \f$x\f$, compute (at most) \f$k\f$ unique indices to
/// be used in the store.
/// \tparam T the type of the item.
/// \tparam x The item to compute the positions for.
/// \return A vector with positions for the store.
template <typename T, typename Core>
typename std::enable_if<
    std::is_same<typename Core::part_type, no_partitioning>::value
  , std::vector<typename Core::store_type::size_type>
>::type
positions(const T& x, const Core& core)
{
    std::vector<typename Core::store_type::size_type> v;
    v.reserve(core.hash.k());

    auto h = core.hash.hash(x);
    for (unsigned i = 0; i < h.size(); ++i)
    {
        auto pos = core.part.position(h[i], i, core.store.size());
        auto j = std::lower_bound(v.begin(), v.end(), pos);
        if (j == v.end() || *j != pos)
            v.insert(j, pos);
    }

    v.shrink_to_fit();
    return v;
}

} // namespace core
} // namespace detail
} // namespace bf

#endif
