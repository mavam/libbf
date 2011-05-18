#ifndef DETAIL_BASIC_H
#define DETAIL_BASIC_H

namespace bf {
namespace detail {
namespace basic {

template <typename T, typename Core>
void add(const T& x, Core& core)
{
    auto pos = core.positions(x);
    for (auto i : pos)
        core.store.increment(i);
}

template <typename T, typename Core>
void add(const T& x, unsigned count, Core& core)
{
    auto pos = core.positions(x);
    for (auto i : pos)
        core.store.increment(i, count);
}

template <typename T, typename Core>
void remove(const T& x, Core& core)
{
    auto pos = core.positions(x);
    for (auto i : pos)
        core.store.decrement(i);
}

} // namespace basic
} // namespace detail
} // namespace bf

#endif
