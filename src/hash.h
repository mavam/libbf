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
  static unsigned get()
  {
    return boost::random_device()();
  }
};

/// A seed policy that fixed seed.
/// \tparam Seed The seed value.
template <unsigned Seed = 42>
struct fixed_seed
{
  static unsigned get()
  {
    return Seed;
  }
};

/// A basic hasher that computes a hash value using boost::hash.
class basic_hasher
{
public:
  typedef std::size_t value_type;

  basic_hasher(std::size_t seed)
    : seed_(seed)
  {
  }

  template <typename T>
  std::size_t operator()(const T& x) const
  {
    std::size_t s = seed_;
    boost::hash_combine(s, x);
    return s;
  }

private:
  std::size_t seed_;
};

/// The base class for hash policies.
/// \tparam The type of the derived policy.
template <typename Derived>
class hash_policy
{
public:
  /// Apply a function \f$k\f$ times to the hash values of \f$x\f$.
  /// \tparam T The type of item \f$x\f$.
  /// \tparam F The unary functor to apply to \f$h_(x)\f$.
  /// \param x The item \f$x\f$.
  /// \param f The functor instance.
  template <typename T, typename F>
  void each(const T& x, F f) const
  {
    for (auto h : derived().hash(x))
      f(h);
  }

  /// Apply a function \f$k\f$ times to \f$h_i(x)\f$ to test whether it is \c
  /// true on \e any of the hash values.
  /// \tparam T The type of item \f$x\f$.
  /// \tparam F The unary functor to apply to \f$(h_i(x), i)\f$.
  /// \param x The item \f$x\f$.
  /// \param f The functor instance.
  /// \return \c true if \e any of the function invocations \f$f(h_i(x))\f$
  ///     is \c true.
  template <typename T, typename F>
    bool any(const T& x, F f) const
    {
      for (auto h : derived().hash(x))
        if (f(h))
          return true;

      return false;
    }

  /// Apply a function \f$k\f$ times to \f$h_i(x)\f$ to test whether it is \c
  /// true on \e all of the hash values.
  /// \tparam T The type of item \f$x\f$.
  /// \tparam F The unary functor to apply to \f$h_i(x)\f$.
  /// \param x The item \f$x\f$.
  /// \param f The functor instance.
  /// \return \c true if \e all of the function invocations \f$f(h_i(x))\f$
  ///     are \c true and \c false otherwise.
  template <typename T, typename F>
  bool all(const T& x, F f) const
  {
    for (auto h : derived().hash(x))
      if (! f(h))
        return false;

    return true;
  }

  /// Apply a function \f$k\f$ times to the pair \f$(h_i(x), i)\f$ where
  /// \f$i\f$ is the index of the \f$i^{\mathrm{th}}\f$ hash function.
  /// \tparam T The type of item \f$x\f$.
  /// \tparam F The binary functor to apply to \f$(h_i(x), i)\f$.
  /// \param x The item \f$x\f$.
  /// \param f The functor instance.
  template <typename T, typename F>
  void each_with_index(const T& x, F f) const
  {
    auto h = derived().hash(x);
    for (unsigned i = 0; i < h.size(); ++i)
      f(h[i], i);
  }

  /// Apply a function \f$k\f$ times to the pair \f$(h_i(x), i)\f$, where
  /// \f$i\f$ is the index of the \f$i^{\mathrm{th}}\f$ hash function, and
  /// return \c true as soon as \f$f\f$ returns \c true for the first time.
  /// Otherwise return \c false.
  /// \tparam T The type of item \f$x\f$.
  /// \tparam F The unary functor to apply to \f$(h_i(x), i)\f$.
  /// \param x The item \f$x\f$.
  /// \param f The functor instance.
  /// \return \c true if \e any of the function invocations \f$f(h_i(x), i)\f$
  ///     is \c true.
  template <typename T, typename F>
  bool any_with_index(const T& x, F f) const
  {
    auto h = derived().hash(x);
    for (unsigned i = 0; i < h.size(); ++i)
      if (f(h[i], i))
        return true;

    return false;
  }

  /// Apply a function \f$k\f$ times to the pair \f$(h_i(x), i)\f$, where
  /// \f$i\f$ is the index of the \f$i^{\mathrm{th}}\f$ hash function, and
  /// return \c false as soon as \f$f\f$ returns \c false for the first time.
  /// Otherwise return \c true.
  /// \tparam T The type of item \f$x\f$.
  /// \tparam F The unary functor to apply to \f$(h_i(x), i)\f$.
  /// \param x The item \f$x\f$.
  /// \param f The functor instance.
  /// \return \c true if \e all the function invocations \f$f(h_i(x), i)\f$
  ///     are \c true.
  template <typename T, typename F>
  bool all_with_index(const T& x, F f) const
  {
    auto h = derived().hash(x);
    for (unsigned i = 0; i < h.size(); ++i)
      if (! f(h[i], i))
        return false;

    return true;
  }

private:
  //
  // CRTP interface
  //
  Derived& derived()
  {
    return *static_cast<Derived*>(this);
  }

  const Derived& derived() const
  {
    return *static_cast<const Derived*>(this);
  }
};

/// The default hash policy.
/// \tparam Hasher An unary functor that computes the underlying hash value.
/// \tparam Seed The seed for the hasher.
template <typename Hasher = basic_hasher, typename Seed = fixed_seed<42>>
class default_hashing : public hash_policy<default_hashing<Hasher, Seed>>
{
  typedef Hasher hasher;
  typedef Seed seed;
  typedef std::vector<hasher> hasher_vector;
public:
  typedef typename hasher::value_type value_type;
  typedef std::vector<value_type> hash_vector;

public:
  default_hashing(unsigned k)
  {
    if (k == 0)
      throw std::invalid_argument("zero hash functions");

    for (unsigned i = 0; i < k; ++i)
      hashers_.push_back(hasher(seed::get() + i));
  }

  unsigned k() const
  {
    return hashers_.size();
  }

  template <typename T>
  hash_vector hash(const T&x) const
  {
    hash_vector h(k(), 0);
    for (typename hasher_vector::size_type i = 0; i < k(); ++i)
      h[i] = hash(i, x);

    return h;
  }

private:
  template <typename T>
    value_type hash(unsigned i, const T& x) const
    {
      return hashers_[i](x);
    }

  hasher_vector hashers_;
};

/// A hash policy that implements <em>double hashing</em>.
/// \tparam Hasher An unary functor that computes the underlying hash value.
/// \tparam Seed1 The seed for the first hasher.
/// \tparam Seed2 The seed for the second hasher.
template <
  typename Hasher = basic_hasher,
  typename Seed1 = fixed_seed<42>,
  typename Seed2 = fixed_seed<4711>
>
class double_hashing : public hash_policy<double_hashing<Hasher, Seed1, Seed2>>
{
public:
  typedef Hasher hasher;
  typedef Seed1 seed1;
  typedef Seed2 seed2;
  typedef typename hasher::value_type value_type;
  typedef std::vector<value_type> hash_vector;

public:
  double_hashing(unsigned k)
    : k_(k)
    , h1_(seed1::get())
    , h2_(seed2::get())
  {
    if (k == 0)
      throw std::invalid_argument("zero hash functions");
  }

  unsigned k() const
  {
    return k_;
  }

  template <typename T>
  hash_vector hash(const T&x) const
  {
    auto h1 = h1_(x);
    auto h2 = h2_(x);

    hash_vector h(k(), 0);
    for (unsigned i = 0; i < k_; ++i)
      h[i] = hash(i, h1, h2);

    return h;
  }

protected:
  value_type hash(unsigned i, value_type h1, value_type h2) const
  {
    return h1 + i * h2;
  }

private:
  unsigned k_;
  hasher h1_;
  hasher h2_;
};

/// A hash policy that implements <em>extended double hashing</em>.
/// \tparam Hasher An unary functor that computes the underlying hash value.
/// \tparam Seed1 The seed for the first hasher.
/// \tparam Seed2 The seed for the second hasher.
template <
  typename Hasher = basic_hasher,
  typename Seed1 = fixed_seed<42>,
  typename Seed2 = fixed_seed<4711>
>
class extended_double_hashing : public double_hashing<Hasher, Seed1, Seed2>
{
  typedef double_hashing<Hasher, Seed1, Seed2> base;
  typedef typename base::value_type value_type;
public:
  typedef std::function<value_type(unsigned)> functor_type;

public:
  extended_double_hashing(unsigned k, functor_type f)
    : double_hashing<Hasher, Seed1, Seed2>(k)
    , f_(f)
  {
  }

protected:
  value_type hash(unsigned i, value_type h1, value_type h2) const
  {
    return h1 + i * h1 + f_(i);
  }

private:
  functor_type f_;
};

} // namespace bf

#endif
