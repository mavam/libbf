#include "bloom_filter/basic.h"

#include <cmath>

namespace bf {

size_t basic_bloom_filter::m(double fp, size_t capacity)
{
  auto ln2 = std::log(2);
  return std::ceil(-(capacity * std::log(fp) / ln2 / ln2));
}

size_t basic_bloom_filter::k(size_t cells, size_t capacity)
{
  auto frac = static_cast<double>(cells) / static_cast<double>(capacity);
  return std::ceil(frac * std::log(2));
}

basic_bloom_filter::basic_bloom_filter(basic_bloom_filter&& other)
  : hasher_(std::move(other.hasher_)),
    bits_(std::move(other.bits_))
{
}

void basic_bloom_filter::add(object const& o)
{
  for (auto d : hasher_(o))
    bits_.set(d % bits_.size());
}

size_t basic_bloom_filter::lookup(object const& o) const
{
  for (auto d : hasher_(o))
    if (! bits_[d % bits_.size()])
      return 0;
  return 1;
}

void basic_bloom_filter::clear()
{
  bits_.reset();
}

void basic_bloom_filter::remove(object const& o)
{
  for (auto d : hasher_(o))
    bits_.reset(d % bits_.size());
}

void basic_bloom_filter::swap(basic_bloom_filter& other)
{
  using std::swap;
  swap(hasher_, other.hasher_);
  swap(bits_, other.bits_);
}

} // namespace bf
