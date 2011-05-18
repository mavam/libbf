#ifndef BLOOM_FILTER_BASIC_H
#define BLOOM_FILTER_BASIC_H

#include <iosfwd>
#include <vector>
#include "bloom_filter.h"
#include "core.h"
#include "detail/basic.h"
#include "detail/spectral.h"

namespace bf {

/// The <em>basic Bloom filter</em>.
template <typename Core = core<>>
class basic : public bloom_filter<basic<Core>>
{
public:
    typedef Core core_type;

    /// Compute \f$k^*\f$, the optimal number of hash functions for a given
    /// false positive rate \f$f\f$.
    /// \return The optimal number of hash functions.
    static double k(double f)
    {
        return std::floor(- (std::log(f) / std::log(2)));
    }

    /// Compute the capacity of a Bloom filter with respect to a given number
    /// of hash functions and number of cells in the store. The capacity is
    /// defined as the maximum number of items the Bloom filter can hold before
    /// the FP rate can no longer be guaranteed.
    /// \param k The number of hash functions.
    /// \param m The number of cells in the Bloom filter
    /// \return The maximum number of items the Bloom filter can hold.
    static double capacity(unsigned k, unsigned m)
    {
        return std::floor(m / k * std::log(2));
    }

    /// Create a basic Bloom filter.
    /// \param core An rvalue reference to a core.
    basic(core_type&& core)
      : core_(core)
    {
    }

    template <typename T>
    void add(const T& x)
    {
        detail::basic::add(x, core_);
    }

    template <typename T>
    void remove(const T& x)
    {
        detail::basic::remove(x, core_);
    }

    template <typename T>
    unsigned count(const T& x) const
    {
        auto pos = core_.positions(x);
        return detail::spectral::minimum(pos, core_.store);
    }

    void clear()
    {
        core_.store.reset();
    }

    std::string to_string() const
    {
        return core_.store.to_string();
    }

    /// Get the number of hash functions.
    /// \return The number of hash functions.
    unsigned k() const
    {
        return core_.hash.k();
    }

    /// Get the core
    /// \return A reference to the core.
    const core_type& core() const
    {
        return core_;
    }

protected:
    core_type core_;
};

template <
    typename Elem,
    typename Traits,
    typename Core
>
std::basic_ostream<Elem, Traits>& operator<<(
        std::basic_ostream<Elem, Traits>& os, const bf::basic<Core>& b)
{
    os << b.to_string();
    return os;
}

} // namespace bf

#endif
