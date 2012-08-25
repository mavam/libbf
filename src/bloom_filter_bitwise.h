#ifndef BF_BLOOM_FILTER_BITWISE_H
#define BF_BLOOM_FILTER_BITWISE_H

#include "bloom_filter.h"
#include "core.h"
#include "detail/bitwise.h"

namespace bf {

/// The <em>bitwise Bloom filter</em>.
template <typename Core = core<>>
class bitwise : public bloom_filter<bitwise<Core>>
{
public:
  typedef Core core_type;
  typedef std::function<unsigned(unsigned, unsigned)> growth_func_type;
private:
  typedef std::vector<core_type> core_vector;

public:
  /// Create a bitwise Bloom filter.
  /// \param core An rvalue reference to the first core.
  /// \param min_size The minimum size of the Bloom filter that represents
  ///     the most-significant bit.
  bitwise(core_type&& core, unsigned min_size = 128)
    : levels_(1, core)
    , min_size_(min_size)
  {
    assert(min_size > 0);
  }

  template <typename T>
  void add(const T& x)
  {
    detail::bitwise::add(x, levels_, min_size_);
  }

  template <typename T>
  void remove(const T& x)
  {
    detail::bitwise::remove(x, levels_);
  }

  template <typename T>
  unsigned count(const T& x) const
  {
    return detail::bitwise::count(x, levels_);
  }

  unsigned levels() const
  {
    return levels_.size();
  }

  void clear()
  {
    for (auto& core : levels_)
      core.store.reset();
  }

  std::string to_string() const
  {
    std::string str;
    auto i = levels_.size();
    while (i)
    {
      str += levels_[--i].store.to_string();
      if (i)
        str += "\n";
    }

    return str;
  }

private:
  core_vector levels_;
  unsigned min_size_;
};

template <typename Elem, typename Traits, typename Core>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& os, const bf::bitwise<Core>& b)
{
  os << b.to_string();
  return os;
}

} // namespace bf

#endif
