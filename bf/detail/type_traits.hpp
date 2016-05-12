#ifndef BF_DETAIL_TYPE_TRAITS_HPP
#define BF_DETAIL_TYPE_TRAITS_HPP

#include <cstdint>
#include <type_traits>

namespace bf {
namespace detail {

// std::conjunction from C++17
template<class...>
struct conjunction : std::true_type { };

template <class B1>
struct conjunction<B1> : B1 { };

template <class B1, class... Bn>
struct conjunction<B1, Bn...>
  : std::conditional_t<!B1::value, conjunction<Bn...>, B1>  {};

// Computes the sum of its arguments.
template <size_t ...>
struct static_sum;

template <size_t S0, size_t ...SN>
struct static_sum<S0, SN...>
  : std::integral_constant<size_t, S0 + static_sum<SN...>{}> {};

template <>
struct static_sum<> : std::integral_constant<size_t, 0> {};

} // namespace detail
} // namespace bf

#endif
