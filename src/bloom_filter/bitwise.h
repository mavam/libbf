#ifndef BF_BLOOM_FILTER_BITWISE_H
#define BF_BLOOM_FILTER_BITWISE_H

#include "bloom_filter.h"
#include "core.h"
#include "detail/bitwise.h"

namespace bf {

/// The bitwise Bloom filter.
template <typename Core = core<>>
class bitwise : public bloom_filter<bitwise<Core>>
{
public:
  typedef Core core_type;
  typedef std::function<size_t(size_t, size_t)> growth_func_type;
private:
  typedef std::vector<core_type> core_vector;

public:
  /// Create a bitwise Bloom filter.
  /// @param core The first Bloom filter core at level 0.
  /// @param min_size The minimum size of the Bloom filter that represents
  ///     the most-significant bit.
  bitwise(core_type core, size_t min_size = 128)
    : levels_(1, std::move(core)), min_size_(min_size)
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
  size_t count(const T& x) const
  {
    return detail::bitwise::count(x, levels_);
  }

  size_t levels() const
  {
    return levels_.size();
  }

  void clear()
  {
    for (auto& core : levels_)
      core.store.reset();
  }

private:
  friend std::string to_string(bitwise const& bf)
  {
    std::string str;
    auto i = bf.levels_.size();
    while (i)
    {
      str += to_string(bf.levels_[--i]);
      if (i)
        str += '\n';
    }
    return str;
  }

  core_vector levels_;
  size_t min_size_;
};

} // namespace bf

#endif
