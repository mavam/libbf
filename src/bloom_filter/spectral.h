#ifndef BF_BLOOM_FILTER_SPECTRAL_H
#define BF_BLOOM_FILTER_SPECTRAL_H

#include "bloom_filter/basic.h"

namespace bf {

/// The spectral Bloom filter with *minimum increase* (MI) optimization.
template <typename Core = core<>>
class spectral_mi : public bloom_filter<spectral_mi<Core>>
{
  typedef bloom_filter<spectral_mi<Core>> super;
  friend super;

public:
  /// Constructs a *Spectral Bloom Filter* with MI optimization.
  /// @param core The Bloom filter core.
  spectral_mi(Core core)
    : core_(std::move(core))
  {
  }

private:
  template <typename T>
  void add_impl(const T& x)
  {
    detail::spectral::minimum_increase(x, core_);
  }

  template <typename T>
  size_t count_impl(const T& x) const
  {
    return detail::spectral::minimum(core_.positions(x), core_.store);
  }

  void clear_impl()
  {
    core_.store.reset();
  }

  friend std::string to_string(spectral_mi const& bf)
  {
    return to_string(bf.core_);
  }

  Core core_;
};

/// The spectral Bloom filter with *recurring minimum* (RM) optimization.
template <typename Core1 = core<>, typename Core2 = Core1>
class spectral_rm : public bloom_filter<spectral_rm<Core1, Core2>>
{
  typedef bloom_filter<spectral_rm<Core1, Core2>> super;
  friend super;

public:
  typedef Core1 core1_type;
  typedef Core2 core2_type;

  /// Create a spectral Bloom filter (RM).
  /// @param core1 The first Bloom filter core.
  /// @param core2 The second Bloom filter core.
  spectral_rm(core1_type core1, core2_type core2)
    : core1_(core1)
    , core2_(core2)
  {
  }

  template <typename T>
  void remove(const T& x)
  {
    detail::spectral::recurring_minimum_remove(x, core1_, core2_);
  }

private:
  template <typename T>
  void add_impl(const T& x)
  {
    detail::spectral::recurring_minimum_add(x, core1_, core2_);
  }

  template <typename T>
  size_t count_impl(const T& x) const
  {
    return detail::spectral::recurring_minimum_count(x, core1_, core2_);
  }

  void clear_impl()
  {
    core1_.store.reset();
    core2_.store.reset();
  }

  friend std::string to_string(spectral_rm const& bf)
  {
    auto str = to_string(bf.core1_);
    str += '\n';
    str += to_string(bf.core2_);
    return str;
  }

private:
  core1_type core1_;
  core2_type core2_;
};

} // namespace bf

#endif
