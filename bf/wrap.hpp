#ifndef BF_WRAP_HPP
#define BF_WRAP_HPP

#include <string>
#include <type_traits>
#include <vector>
#include <bf/object.hpp>

namespace bf {

template <
  typename T,
  typename = typename std::enable_if<std::is_arithmetic<T>::value>::type
>
object wrap(T const& x)
{
  return {&x, sizeof(T)};
}

template <
  typename T,
  size_t N,
  typename = typename std::enable_if<std::is_arithmetic<T>::value>::type
>
object wrap(T const (&str)[N])
{
  return {&str, N * sizeof(T)};
}

template <
  typename T,
  typename = typename std::enable_if<std::is_arithmetic<T>::value>::type
>
object wrap(std::vector<T> const& s)
{
  return {s.data(), s.size()};
}

inline object wrap(std::string const& str)
{
  return {str.data(), str.size()};
}

} // namespace bf

#endif
