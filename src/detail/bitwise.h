#ifndef DETAIL_BITWISE_H
#define DETAIL_BITWISE_H

#include "detail/basic.h"
#include "detail/spectral.h"

namespace bf {
namespace detail {
namespace bitwise {

template <typename T, typename V>
void add(const T& x, V& levels, unsigned min_size)
{
    typedef std::vector<typename V::value_type::store_type::pos_vector> vector;
    vector pos;
    for (typename V::size_type i = 0; i < levels.size(); ++i)
    {
        pos.push_back(levels[i].positions(x));
        auto cnt = detail::spectral::minimum(pos[i], levels[i].store);
        if (cnt < levels[i].store.max())
        {
            for (auto j : pos[i])
                levels[i].store.increment(j);

            return;
        }

        for (auto j : pos[i])
            levels[i].store.reset(j);
    }

    auto z = levels.back();
    auto cells = z.store.size() / 2;    // TODO: make growth rate configurable.
    if (cells < min_size)
        cells = min_size;

    levels.push_back({ cells, z.hash.k(), z.store.width(), z.part.parts() });
    detail::basic::add(x, levels.back());
}

template <typename T, typename V>
void remove(const T& x, V& levels)
{
    typedef std::vector<typename V::value_type::store_type::pos_vector> vector;
    vector pos;
    for (typename V::size_type i = 0; i < levels.size(); ++i)
    {
        pos.push_back(levels[i].positions(x));
        auto cnt = detail::spectral::minimum(pos[i], levels[i].store);
        if (cnt > 0)
        {
            for (auto j : pos[i])
                levels[i].store.decrement(j);

            if (levels.size() > 1 && levels.back().store.none())
                levels.pop_back();

            return;
        }

        if (levels.size() > 1)
            for (auto j : pos[i])
                levels[i].store.set(j);
    }
}

template <typename T, typename V>
unsigned count(const T& x, V& levels)
{
    unsigned width = 0;
    unsigned result = 0;
    for (typename V::size_type i = 0; i < levels.size(); ++i)
    {
        auto& core = levels[i];
        auto cnt = detail::spectral::minimum(core.positions(x), core.store);

        if (cnt)
            result += cnt << width;

        width += core.store.width();
    }

    return result;
}

} // namespace bitwise
} // namespace detail
} // namespace bf

#endif
