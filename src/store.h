#ifndef BF_STORE_H
#define BF_STORE_H

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <boost/dynamic_bitset.hpp>

namespace bf {

/// The base class for store policies.
/// \tparam The type of the derived policy.
template <typename Derived>
class store_policy
{
public:
  // TODO: decide on a reasonable interface.

private:
  //
  // CRTP interface
  //
  Derived& derived()
  {
    return *static_cast<Derived*>(this);
  }

  const Derived& derived() const
  {
    return *static_cast<const Derived*>(this);
  }
};

/// The <em>fixed width</em> storage policy implements a bit vector where each
/// cell itself is a counter with a fixed number of bits.
template <typename Block, typename Allocator>
class fixed_width : public store_policy<fixed_width<Block, Allocator>>
{
public:
  typedef Block block_type;
  typedef Allocator allocator_type;

  typedef boost::dynamic_bitset<Block, Allocator> bitset;
  typedef typename bitset::size_type size_type;
  typedef std::vector<size_type> pos_vector;
  typedef uint64_t count_type;

public:
  /// Construct a bit vector of size \f$O(mw)\f$ where \f$m\f$ is the
  /// number of cells and \f$w\f$ the number of bits per cell.
  /// \param cells The number of cells in the bit vector.
  /// \param width The number of pits per cell.
  fixed_width(unsigned cells, unsigned width)
    : bits_(cells * width)
    , width_(width)
  {
    if (cells == 0)
      throw std::invalid_argument("zero cells");

    if (width == 0)
      throw std::invalid_argument("zero width");

    auto max = std::numeric_limits<count_type>::digits;
    if (width > static_cast<decltype(width)>(max))
      throw std::invalid_argument("width too large");
  }

  template <typename B, typename A>
  void swap(fixed_width<B, A>& store) // no throw
  {
    std::swap(bits_, store.bits_);
    std::swap(width_, store.width_);
  }

  /// Increment a cell counter.
  /// \param cell The cell index.
  /// \return \c true if the increment succeeded, \c false if all bits in the
  ///     cell were already 1.
  bool increment(size_type cell)
  {
    assert(cell < size());

    size_type lsb = cell * width_;
    for (auto i = lsb; i < lsb + width_; ++i)
      if (! bits_[i])
      {
        bits_[i] = true;
        while (i && i > lsb)
          bits_[--i] = false;

        return true;
      }

    return false;
  }

  /// Increment a cell counter by a given value. If the value is larger 
  /// than or equal to max(), All bits are set to 1.
  /// \param cell The cell index.
  /// \param value The value that is added to the current cell value.
  /// \return - \c true if the increment succeeded, \c false if all bits in
  ///     the cell were already 1.
  bool increment(size_type cell, count_type value)
  {
    assert(cell < size());
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

    bitset b(width_, value);
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

  /// Decrement a cell counter.
  /// \param cell The cell index.
  /// \return \c true if the decrement succeeded, \c false if all bits in the
  ///     cell were already 0.
  bool decrement(size_type cell)
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

  /// Get the count of a cell.
  /// \param cell The cell index.
  /// \return \c true if the decrement succeeded, \c false if all bits in the
  ///     cell were already 0.
  count_type count(size_type cell) const
  {
    assert(cell < size());
    count_type cnt = 0, order = 1;
    size_type lsb = cell * width_;
    for (auto i = lsb; i < lsb + width_; ++i, order <<= 1)
      if (bits_[i])
        cnt |= order;

    return cnt;
  }

  /// Set every bit in the bit vector.
  void set()
  {
    bits_.set();
  }

  /// Set a cell to all 1s.
  /// \param cell The cell to set to all 1s.
  void set(size_type cell)
  {
    assert(cell < size());
    auto lsb = cell * width_;
    for (auto i = lsb; i < lsb + width_; ++i)
      bits_[i] = true;
  }

  /// Set a cell to a given value.
  /// \param cell The cell whose value changes.
  /// \param value The new value of the cell.
  void set(size_type cell, count_type value)
  {
    assert(cell < size());
    assert(value <= max());
    bitset bits(width_, value);
    size_type i = cell * width_;
    for (auto bit : bits)
      bits_[i++] = bit;
  }

  /// Clear every bit in the bit vector.
  void reset()
  {
    bits_.reset();
  }

  /// Clear the bit(s) of specific cell.
  /// \param cell The cell to clear.
  void reset(size_type cell)
  {
    assert(cell < size());

    auto lsb = cell * width_;
    for (auto i = lsb; i < lsb + width_; ++i)
      bits_[i] = false;
  }

  /// Shrink the number of cells in the bit vector by a factor of 2.
  void halve()
  {
    assert(bits_.size() % 2 == 0);
    assert(bits_.size() > 0);
    size_type half = bits_.size() / 2;
    size_type i = 0;
    size_type j = half;

    while (i < half)
      set(i, count(i) + count(j));

    bits_.resize(half * width_);
  }

  /// Get the size of the underlying bit vector.
  /// \return The size of the bit vector in number of cells.
  size_type size() const
  {
    return bits_.size() / width_;
  }

  /// Get the maximum counter value this store supports.
  /// \return The maximum counter value.
  count_type max() const
  {
    return std::numeric_limits<count_type>::max() >>
      (std::numeric_limits<count_type>::digits - width());
  }

  /// Test whether all bits are 0.
  /// \return \c true \e iff all bits in the bit vector are 0.
  bool none() const
  {
    return bits_.none();
  }

  /// Get the counter width of a cell.
  /// \return The number of bits per cell.
  unsigned width() const
  {
    return width_;
  }

  /// Set the cell width.
  /// \param w The new value of the cell width.
  /// \todo Write unit tests.
  void width(unsigned w)
  {
    assert(! "not yet tested");
    if (w < width_)
    {
      size_type i = 0;
      for (auto cell = 0; cell < bits_.size(); cell += width_)
        if (bits_[cell + width_])
          for (auto j = 0; j < w; ++j)
            bits_[i++] = true;
        else
          for (auto j = 0; j < w; ++j)
            bits_[i++] = bits_[cell + j];

      bits_.resize(w * bits_.size());
    }
    else
    {
      auto old = bits_.size();
      bits_.resize(w * bits_.size());
      auto i = bits_.size() / w - 1;

      for (auto cell = old - width_ - 1; cell != 0; cell -= width_)
      {
        size_type j = 0;
        while (j++ < width_)
          bits_[i++] = bits_[cell + j];
        while (j++ < w)
          bits_[i++] = 0;

        i -= width_;
      }
    }

    width_ = w;
  }

  /// Get a string representation of the storage. The output reads from left
  /// to right. That is, for each cell the least-significant bit corresponds
  /// to the left-most bit.
  /// \param A string of the underlying bit vector.
  std::string to_string() const
  {
    std::string str(bits_.size(), '0');
    for (size_type i = 0; i < bits_.size(); ++i)
      if (bits_[i])
        str[i] = '1';

    return str;
  }

  /// Apply a functor to each counter in the bit vector.
  /// \tparam The type of the unary functor.
  /// \param f An instance of type F.
  template <typename F>
  void each(F f) const
  {
    for (size_type cell = 0; cell < bits_.size() / width_; ++cell)
      f(count(cell));
  }

private:
  bitset bits_;
  unsigned width_;
};

} // namespace bf

#endif
