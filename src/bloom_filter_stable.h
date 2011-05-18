#ifndef BLOOM_FILTER_STABLE_H
#define BLOOM_FILTER_STABLE_H

#include <random>
#include "bloom_filter_basic.h"
#include "detail/evict.h"

namespace bf {

/// The <em>stable Bloom filter</em>.
template <typename Core = core<>, typename Generator = std::mt19937>
class stable : public basic<Core>
{
    typedef Generator generator_type;
    typedef basic<Core> base;

public:
    typedef typename base::core_type core_type;

    /// Create a spectral Bloom filter.
    /// \param core An rvalue reference to a core.
    /// \param generator An lvalue reference to a randomness generator.
    stable(core_type&& core, const generator_type& generator)
      : base(std::forward<core_type>(core))
      , generator_(generator)
    {
    }

    /// Add an item to the stable Bloom filter. This invovles 
    /// (i) decrementing \e k positions uniformly at random and (ii) setting
    /// the counter of the value \c x to its maximum value, i.e., all 1's.
    template <typename T>
    void add(const T& x)
    {
        detail::evict::random(base::core_, generator_);

        auto beyond = 1 << base::core_.store.width();
        detail::basic::add(x, beyond, base::core_);
    }

    // Spectral bloom filters do not support deletion of items.
    template <typename T>
    void remove(const T& x) = delete;

private:
    /// Generates randomness out of thin air.
    generator_type generator_;
};

} // namespace bf

#endif
