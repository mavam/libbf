#ifndef DETAIL_EVICT_H
#define DETAIL_EVICT_H

namespace bf {
namespace detail {
namespace evict {

template <typename Core, typename Generator>
void random(Core& core, Generator& generator, unsigned d)
{
  while (d--)
  {
    std::uniform_int_distribution<> pdf(0, core.store.size() - 1);
    core.store.decrement(pdf(generator));
  }
}

} // namespace basic
} // namespace detail
} // namespace bf

#endif
