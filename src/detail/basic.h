#ifndef BF_DETAIL_BASIC_H
#define BF_DETAIL_BASIC_H

namespace bf {
namespace detail {
namespace basic {

template <typename T, typename Core>
void add(const T& x, Core& core)
{
  for (auto i : core.positions(x))
    core.store.increment(i);
}

template <typename T, typename Core>
void add(const T& x, size_t count, Core& core)
{
  for (auto i : core.positions(x))
    core.store.increment(i, count);
}

template <typename T, typename Core>
void remove(const T& x, Core& core)
{
  for (auto i : core.positions(x))
    core.store.decrement(i);
}

} // namespace basic
} // namespace detail
} // namespace bf

#endif
