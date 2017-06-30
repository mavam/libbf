#include <bf/bloom_filter/stable.hpp>

#include <cassert>

namespace bf {

stable_bloom_filter::stable_bloom_filter(hasher h, size_t cells, size_t width,
                                         size_t d)
    : counting_bloom_filter(std::move(h), cells, width),
      d_(d),
      unif_(0, cells - 1) {
  assert(d <= cells);
}

void stable_bloom_filter::add(object const& o) {
  // Decrement d distinct cells uniformly at random.
  std::vector<size_t> indices;
  for (size_t d = 0; d < d_; ++d) {
    bool unique;
    do {
      size_t u = unif_(generator_);
      unique = true;
      for (auto i : indices)
        if (i == u) {
          unique = false;
          break;
        }
      if (unique) {
        indices.push_back(u);
        cells_.decrement(u);
      }
    } while (!unique);
  }

  increment(find_indices(o), cells_.max());
}

} // namespace bf
