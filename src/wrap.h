#ifndef BF_WRAP_H
#define BF_WRAP_H

#include <type_traits>
#include "object.h"

namespace bf {

template <
  typename T,
  typename std::enable_if<std::is_integral<T>::value>::type
>
object wrap(T const& x)
{
  return {&x, sizeof(T)};
}

template <typename Sequence>
object wrap(Sequence const& s)
{
  return {s.data(), s.size()};
}

} // namespace bf

#endif
