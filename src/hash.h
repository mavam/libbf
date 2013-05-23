#ifndef BF_HASH_H
#define BF_HASH_H

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <boost/functional/hash.hpp>
#include <boost/nondet_random.hpp>

namespace bf {

/// A seed policy that returns a non-deterministic random seed.
struct random_seed
{
  static size_t get()
  {
    return boost::random_device()();
  }
};

/// A seed policy that fixed seed.
/// @tparam Seed The seed value.
template <size_t Seed = 42>
struct fixed_seed
{
  static size_t get()
  {
    return Seed;
  }
};

/// A basic hasher that computes a hash value using boost::hash.
class basic_hasher
{
public:
  typedef size_t value_type;

  basic_hasher(size_t seed)
    : seed_(seed)
  {
  }

  template <typename T>
  size_t operator()(T const& x) const
  {
    size_t s = seed_;
    boost::hash_combine(s, x);
    return s;
  }

private:
  size_t seed_;
};

/// The base class for hash policies.
/// @tparam The type of the derived policy.
template <typename Derived>
class hash_policy
{
public:
  /// Apply a function @f$k@f$ times to the hash values of @f$x@f$.
  /// @tparam T The type of item @f$x@f$.
  /// @tparam F The unary functor to apply to @f$h_(x)@f$.
  /// @param x The item @f$x@f$.
  /// @param f The functor instance.
  template <typename T, typename F>
  void each(T const& x, F f) const
  {
    for (auto h : derived().hash(x))
      f(h);
  }

  /// Apply a function @f$k@f$ times to @f$h_i(x)@f$ to test whether it is
  /// `true` on *any* of the hash values.
  /// @tparam T The type of item @f$x\@$.
  /// @tparam F The unary functor to apply to @f$(h_i(x), i)@f$.
  /// @param x The item @f$x@f$.
  /// @param f The functor instance.
  /// @return `true` if *any* of the function invocations @f$f(h_i(x))@f$ is
  /// `true`.
  template <typename T, typename F>
  bool any(T const& x, F f) const
  {
    for (auto h : derived().hash(x))
      if (f(h))
        return true;
    return false;
  }

  /// Apply a function @f$k@f$ times to @f$h_i(x)@f$ to test whether it is \c
  /// true on \e all of the hash values.
  /// @tparam T The type of item @f$x@f$.
  /// @tparam F The unary functor to apply to @f$h_i(x)@f$.
  /// @param x The item @f$x@f$.
  /// @param f The functor instance.
  /// @return `true` if *all* of the function invocations @f$f(h_i(x))@f$
  ///     are `true` and `false` otherwise.
  template <typename T, typename F>
  bool all(T const& x, F f) const
  {
    for (auto h : derived().hash(x))
      if (! f(h))
        return false;
    return true;
  }

  /// Apply a function @f$k@f$ times to the pair @f$(h_i(x), i)@f$ where
  /// @f$i@f$ is the index of the @f$i^{\mathrm{th}}@f$ hash function.
  /// @tparam T The type of item @f$x@f$.
  /// @tparam F The binary functor to apply to @f$(h_i(x), i)@f$.
  /// @param x The item @f$x@f$.
  /// @param f The functor instance.
  template <typename T, typename F>
  void each_with_index(T const& x, F f) const
  {
    auto h = derived().hash(x);
    for (size_t i = 0; i < h.size(); ++i)
      f(h[i], i);
  }

  /// Apply a function @f$k@f$ times to the pair @f$(h_i(x), i)@f$, where
  /// @f$i@f$ is the index of the @f$i^{\mathrm{th}}@f$ hash function, and
  /// return `true` as soon as @f$f@f$ returns `true` for the first time.
  /// Otherwise return `false`.
  /// @tparam T The type of item @f$x@f$.
  /// @tparam F The unary functor to apply to @f$(h_i(x), i)@f$.
  /// @param x The item @f$x@f$.
  /// @param f The functor instance.
  /// @return `true` if *any* of the function invocations @f$f(h_i(x), i)@f$
  /// `true`.
  template <typename T, typename F>
  bool any_with_index(T const& x, F f) const
  {
    auto h = derived().hash(x);
    for (size_t i = 0; i < h.size(); ++i)
      if (f(h[i], i))
        return true;
    return false;
  }

  /// Apply a function @f$k@f$ times to the pair @f$(h_i(x), i)@f$, where
  /// @f$i@f$ is the index of the @f$i^{\mathrm{th}}@f$ hash function, and
  /// return `false` as soon as @f$f@f$ returns `false` for the first time.
  /// Otherwise return `true`.
  /// @tparam T The type of item @f$x@f$.
  /// @tparam F The unary functor to apply to @f$(h_i(x), i)@f$.
  /// @param x The item @f$x@f$.
  /// @param f The functor instance.
  /// @return `true. if *all* the function invocations @f$f(h_i(x), i)@f$
  ///     are `true`.
  template <typename T, typename F>
  bool all_with_index(T const& x, F f) const
  {
    auto h = derived().hash(x);
    for (size_t i = 0; i < h.size(); ++i)
      if (! f(h[i], i))
        return false;
    return true;
  }

private:
  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  const Derived& derived() const
  {
    return static_cast<Derived const&>(*this);
  }
};

/// The default hash policy.
/// @tparam Hasher An unary functor that computes the underlying hash value.
/// @tparam Seed The seed for the hasher.
template <typename Hasher = basic_hasher, typename Seed = fixed_seed<>>
class default_hashing : public hash_policy<default_hashing<Hasher, Seed>>
{
  typedef Hasher hasher;
  typedef Seed seed;
public:
  typedef typename hasher::value_type value_type;

public:
  default_hashing(size_t k)
  {
    if (k == 0)
      throw std::invalid_argument("zero hash functions");
    for (size_t i = 0; i < k; ++i)
      hashers_.push_back(hasher(seed::get() + i));
  }

  size_t k() const
  {
    return hashers_.size();
  }

  template <typename T>
  std::vector<value_type> hash(T const&x) const
  {
    std::vector<value_type> h(k(), 0);
    for (typename std::vector<hasher>::size_type i = 0; i < k(); ++i)
      h[i] = hash(i, x);
    return h;
  }

private:
  template <typename T>
  value_type hash(size_t i, T const& x) const
  {
    return hashers_[i](x);
  }

  std::vector<hasher> hashers_;
};

/// A hash policy that implements *double hashing*.
/// @tparam Hasher An unary functor that computes the underlying hash value.
/// @tparam Seed1 The seed for the first hasher.
/// @tparam Seed2 The seed for the second hasher.
template <
  typename Hasher = basic_hasher,
  typename Seed1 = fixed_seed<>,
  typename Seed2 = fixed_seed<>
>
class double_hashing : public hash_policy<double_hashing<Hasher, Seed1, Seed2>>
{
public:
  typedef Hasher hasher;
  typedef Seed1 seed1;
  typedef Seed2 seed2;
  typedef typename hasher::value_type value_type;

public:
  double_hashing(size_t k)
    : k_(k), h1_(seed1::get()), h2_(seed2::get())
  {
    if (k == 0)
      throw std::invalid_argument("zero hash functions");
  }

  size_t k() const
  {
    return k_;
  }

  template <typename T>
  std::vector<value_type> hash(T const&x) const
  {
    auto h1 = h1_(x);
    auto h2 = h2_(x);
    std::vector<value_type> h(k(), 0);
    for (size_t i = 0; i < k_; ++i)
      h[i] = hash(i, h1, h2);
    return h;
  }

protected:
  value_type hash(size_t i, value_type h1, value_type h2) const
  {
    return h1 + i * h2;
  }

private:
  size_t k_;
  hasher h1_;
  hasher h2_;
};

/// A hash policy that implements *extended double hashing*.
/// @tparam Hasher An unary functor that computes the underlying hash value.
/// @tparam Seed1 The seed for the first hasher.
/// @tparam Seed2 The seed for the second hasher.
template <
  typename Hasher = basic_hasher,
  typename Seed1 = fixed_seed<>,
  typename Seed2 = fixed_seed<>
>
class extended_double_hashing : public double_hashing<Hasher, Seed1, Seed2>
{
  typedef double_hashing<Hasher, Seed1, Seed2> base;
  typedef typename base::value_type value_type;
public:
  typedef std::function<value_type(size_t)> functor_type;

public:
  extended_double_hashing(size_t k, functor_type f)
    : double_hashing<Hasher, Seed1, Seed2>(k)
    , f_(f)
  {
  }

protected:
  value_type hash(size_t i, value_type h1, value_type h2) const
  {
    return h1 + i * h1 + f_(i);
  }

private:
  functor_type f_;
};

} // namespace bf

#endif
