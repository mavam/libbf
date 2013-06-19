#ifndef BF_HASH_POLICY_H
#define BF_HASH_POLICY_H

#include <functional>
#include "object.h"

namespace bf {

/// The hash digest type.
typedef size_t digest;

/// The hash function type.
typedef std::function<digest(object const&)> hash_function;

/// A function that hashes an object *k* times.
typedef std::function<std::vector<digest>(object const&)> hasher;

/// A hasher which hashes an object *k* times.
class default_hasher
{
public:
  default_hasher(std::vector<hash_function> fns)
    : fns_(std::move(fns))
  {
  }

  std::vector<digest> operator()(object const& o) const
  {
    std::vector<digest> d(fns_.size());
    for (size_t i = 0; i < fns_.size(); ++i)
      d[i] = fns_[i](o);
    return d;
  }

private:
  std::vector<hash_function> fns_;
};

/// A hasher which hashes an object two times and generates *k* digests through
/// a linear combinations of the two digests.
class double_hasher
{
public:
  double_hasher(size_t k, hash_function h1, hash_function h2)
    : k_(k),
      h1_(std::move(h1)),
      h2_(std::move(h2))
  {
  }

  std::vector<digest> operator()(object const& o) const
  {
    auto d1 = h1_(o);
    auto d2 = h2_(o);
    std::vector<digest> d(k_);
    for (size_t i = 0; i < d.size(); ++i)
      d[i] = d1 + i * d2;
    return d;
  }

private:
  size_t k_;
  hash_function h1_;
  hash_function h2_;
};

} // namespace bf

#endif
