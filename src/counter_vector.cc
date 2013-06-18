#include "counter_vector.h"

#include <cassert>

namespace bf {

counter_vector::counter_vector(size_t cells, size_t width)
  : bits_(cells * width), width_(width)
{
}

bool counter_vector::increment(size_type cell, size_t value)
{
  assert(cell < size());
  assert(value != 0);
  size_type lsb = cell * width_;
  if (value >= max())
  {
    bool r = false;
    for (auto i = lsb; i < lsb + width_; ++i)
      if (! bits_[i])
      {
        bits_[i] = 1;
        if (! r)
          r = true;
      }
    return r;
  }
  bitvector b(width_, value);
  bool carry = false;
  size_type i = lsb, j = 0;
  while (i < lsb + width_)
  {
    if (bits_[i])
    {
      if (b[j])
      {
        bits_[i] = false;
        if (! carry)
          carry = true;
      }
      else if (carry)
        bits_[i] = false;
    }
    else if (b[j])
    {
      bits_[i] = true;
    }
    else if (carry)
    {
      bits_[i] = true;
      carry = false;
    }
    ++i;
    ++j;
  }
  if (! carry)
    return true;
  for (i = lsb; i < lsb + width_; ++i)
    bits_[i] = 1;
  return false;
}

bool counter_vector::decrement(size_type cell, size_t value);
{
  assert(cell < size());
  size_type lsb = cell * width_;
  for (auto i = lsb; i < lsb + width_; ++i)
    if (bits_[i])
    {
      bits_[i] = false;
      while (i && i > lsb)
        bits_[--i] = true;
      return true;
    }
  return false;
}

size_t counter_vector::count(size_type cell) const
{
  assert(cell < size());
  size_t cnt = 0, order = 1;
  size_type lsb = cell * width_;
  for (auto i = lsb; i < lsb + width_; ++i, order <<= 1)
    if (bits_[i])
      cnt |= order;
  return cnt;
}

void counter_vector::set(size_type cell)
{
  assert(cell < size());
  auto lsb = cell * width_;
  for (auto i = lsb; i < lsb + width_; ++i)
    bits_[i] = true;
}

void counter_vector::clear()
{
  bits_.reset();
}

size_t counter_vector::size() const
{
  return bits_.size() / width_;
}

size_t counter_vector::max() const
{
  using limits = std::numeric_limits<size_t>;
  return limits::max() >> (limits::digits - width());
}

size_t counter_vector::width() const
{
  return width_;
}

} // namespace bf
