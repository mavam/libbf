#ifndef DETAIL_SPECTRAL_H
#define DETAIL_SPECTRAL_H

#include <tuple>
#include "detail/basic.h"

namespace bf {
namespace detail {
namespace spectral {

/// Get the minimum count of an item. This quering algorithm is dubbed
/// <em>minimum selection</em> (MS) in the spectral Bloom filter paper.
/// \tparam Store The store type.
/// \param pos A vector of cell indices of a store.
/// \param store An reference to a store.
/// \return The minimum count of the amongst the postions in pos.
template <typename Store>
unsigned 
minimum(const typename Store::pos_vector& pos, const Store& store)
{
    auto min = store.max();
    for (auto i : pos)
    {
        auto cnt = store.count(i);
        if (cnt < min)
            min = cnt;

        if (min == 0)
            break;
    }

    return min;
}

/// Get the positions of all minima.
/// \tparam Store The store type.
/// \param pos A vector of cell indices of a store.
/// \param store An reference to a store.
/// \return a tuple <em>(min, rec, pos)</em> where \e min is the value of the
///     minimum, \e rec a boolean flag indicating whether the minimum is
///     recurring, and \e pos the vector containing all the positions of it. 
/// \note The \e rec flag is necessary because simply inspecting the number of
///     minima does not suffice: in case all \f$k\f$ hash functions yield the
///     same store position, we necessarily end up with one minimum and define
///     this to be a recurring one.
template <typename Store>
std::tuple<typename Store::size_type, bool, typename Store::pos_vector>
minima(const typename Store::pos_vector& pos, const Store& store)
{
    typedef std::tuple<
        typename Store::size_type
      , bool
      , typename Store::pos_vector
    > result_type;

    auto min = store.max();
    typename Store::pos_vector pos_min;
    for (auto i : pos)
    {
        auto cnt = store.count(i);

        if (cnt == min)
            pos_min.push_back(i);
        else if (cnt < min)
        {
            min = cnt;
            pos_min.clear();
            pos_min.push_back(i);
        }
    }

//    pos_min.shrink_to_fit();
    bool recurring = pos_min.size() > 1 || pos.size() == 1;

    return result_type{ min, recurring, pos_min };
}

/// Increase the cell of the minimum item. If there is no unique minimum then
/// increase all cells with the minimum. This quering algorithm is dubbed
/// <em>minimum increase</em> (MI) in the spectral Bloom filter paper.
/// \tparam T The type of the item to query.
/// \tparam Core The core type.
/// \param x An instance of type T.
/// \param core An instance of the core.
template <typename T, typename Core>
void minimum_increase(const T& x, Core& core)
{
    auto pos = core.positions(x);
    for (auto i : std::get<2>(minima(pos, core.store)))
        core.store.increment(i);
}

/// Get the count of an item to according to the <em>recurring minimum</em> (RM)
/// algorithm from the spectral Bloom filter.
/// \tparam T The type of the item to query.
/// \tparam Core1 The type of the first core.
/// \tparam Core2 The type of the second core.
/// \param x An instance of type T.
/// \param core1 An instance of the first core.
/// \param core2 An instance of the second core.
/// \return A frequency estimate for x.
template <typename T, typename Core1, typename Core2>
unsigned
recurring_minimum_count(const T& x, const Core1& core1, const Core2& core2)
{
    auto min = minima(core1.positions(x), core1.store);
    if (std::get<1>(min))
        return std::get<0>(min);

    auto min2 = minimum(core2.positions(x), core2.store);
    if (min2)
        return min2;

    return std::get<0>(min);
}

/// Add an item to the multiset according to the <em>minimum increase</em> (MI)
/// algorithm from the spectral Bloom filter.
/// \tparam T The type of the item to query.
/// \tparam Core1 The type of the first core.
/// \tparam Core2 The type of the second core.
/// \param x An instance of type T.
/// \param core1 An instance of the first core.
/// \param core2 An instance of the second core.
template <typename T, typename Core1, typename Core2>
void recurring_minimum_add(const T& x, Core1& core1, Core2& core2)
{
    auto pos = core1.positions(x);
    for (auto i : pos)
        core1.store.increment(i);

    auto min = minima(pos, core1.store);
    if (std::get<1>(min))
        return;

    pos = core2.positions(x);
    typename Core2::store_type::size_type i = 0;
    while (i < pos.size())
    {
        if (core2.store.count(i) == 0)
            break;

        ++i;
    }

    if (i == pos.size())
        for (auto i : pos)
            core2.store.increment(i);
    else
        for (auto i : pos)
            core2.store.increment(i, std::get<0>(min));
}

/// Remove an item from the multiset according to the
/// <em>minimum increase</em> (MI) algorithm from the spectral Bloom filter.
/// \tparam T The type of the item to query.
/// \tparam Core1 The type of the first core.
/// \tparam Core2 The type of the second core.
/// \param x An instance of type T.
/// \param core1 An instance of the first core.
/// \param core2 An instance of the second core.
template <typename T, typename Core1, typename Core2>
void recurring_minimum_remove(const T& x, Core1& core1, Core2& core2)
{
    auto pos = core1.positions(x);
    for (auto i : pos)
        core1.store.decrement(i);

    auto min = minima(pos, core1.store);
    if (std::get<1>(min))
        return;

    pos = core2.positions(x);
    typename Core2::store_type::size_type i = 0;
    while (i < pos.size())
    {
        if (core2.store.count(i) == 0)
            break;

        ++i;
    }

    if (i == pos.size())
        for (auto i : pos)
            core2.store.decrement(i);
}

} // namespace spectral
} // namespace detail
} // namespace bf

#endif
