#ifndef BLOOM_FILTER_SPECTRAL_H
#define BLOOM_FILTER_SPECTRAL_H

#include "bloom_filter_basic.h"

namespace bf {

/// The <em>spectral Bloom filter</em> with <em>minimum increase</em>
/// optimization.
template <typename Core = core<>>
class spectral_mi : public basic<Core>
{
    typedef basic<Core> base;
public:
    typedef typename base::core_type core_type;

    /// Create a spectral Bloom filter.
    /// \param core An rvalue reference to a core object.
    spectral_mi(core_type&& core)
      : base(std::forward<core_type>(core))
    {
    }

    template <typename T>
    void add(const T& x)
    {
        detail::spectral::minimum_increase(x, base::core_);
    }

    template <typename T>
    void remove(const T& x) = delete;
};

/// The <em>spectral Bloom filter</em> with <em>recurring minimum</em>
/// optimization.
template <typename Core1 = core<>, typename Core2 = Core1>
class spectral_rm : public bloom_filter<spectral_rm<Core1, Core2>>
{
    typedef bloom_filter<spectral_rm<Core1, Core2>> base;
public:
    typedef Core1 core1_type;
    typedef Core2 core2_type;

    /// Create a spectral Bloom filter (RM).
    /// \param core1 An rvalue reference to the first core.
    /// \param core2 An rvalue reference to the second core.
    spectral_rm(core1_type&& core1, core2_type&& core2)
      : core1_(core1)
      , core2_(core2)
    {
    }

    template <typename T>
    void add(const T& x)
    {
        detail::spectral::recurring_minimum_add(x, core1_, core2_);
    }

    template <typename T>
    void remove(const T& x)
    {
        detail::spectral::recurring_minimum_remove(x, core1_, core2_);
    }

    template <typename T>
    unsigned count(const T& x) const
    {
        return detail::spectral::recurring_minimum_count(x, core1_, core2_);
    }

    void clear()
    {
        core1_.store.reset();
        core2_.store.reset();
    }

    /// Get the first core.
    /// \return A reference to the first core.
    const core1_type& core1() const
    {
        return core1_;
    }

    /// Get the second core.
    /// \return A reference to the second core.
    const core2_type& core2() const
    {
        return core2_;
    }

    std::string to_string() const
    {
        std::string str = core1_.store.to_string();
        str.push_back('\n');
        str += core2_.store.to_string();

        return str;
    }

private:
    core1_type core1_;
    core2_type core2_;
};

} // namespace bf

template <
    typename Elem
  , typename Traits
  , typename Core
>
std::basic_ostream<Elem, Traits>& operator<<(
        std::basic_ostream<Elem, Traits>& os, const bf::spectral_mi<Core>& b)
{
    os << b.to_string();

    return os;
}

template <
    typename Elem
  , typename Traits
  , typename C1
  , typename C2
>
std::basic_ostream<Elem, Traits>& operator<<(
        std::basic_ostream<Elem, Traits>& os, const bf::spectral_rm<C1, C2>& b)
{
    os << b.to_string();

    return os;
}

#endif
