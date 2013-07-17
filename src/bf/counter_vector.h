#ifndef BF_COUNTER_VECTOR_H
#define BF_COUNTER_VECTOR_H

#include <cstdint>
#include <vector>
#include <bf/bitvector.h>

namespace bf {

/// The *fixed width* storage policy implements a bit vector where each
/// cell represents a counter having a fixed number of bits.
class counter_vector
{
  friend std::string to_string(counter_vector const&, bool, size_t);

public:
  /// Construct a counter vector of size @f$O(mw)@f$ where *m is the number of
  /// cells and *w the number of bits per cell.
  ///
  /// @param cells The number of cells.
  ///
  /// @param width The number of bits per cell.
  ///
  /// @pre `cells > 0 && width > 0`
  counter_vector(size_t cells, size_t width);

  /// Increments a cell counter by a given value. If the value is larger 
  /// than or equal to max(), all bits are set to 1.
  ///
  /// @param cell The cell index.
  ///
  /// @param value The value that is added to the current cell value.
  ///
  /// @return `true` if the increment succeeded, `false` if all bits in
  ///     the cell were already 1.
  bool increment(size_t cell, size_t value = 1);

  /// Decrements a cell counter.
  ///
  /// @param cell The cell index.
  ///
  /// @return `true` if decrementing succeeded, `false` if all bits in the
  /// cell were already 0.
  bool decrement(size_t cell, size_t value = 1);

  /// Retrieves the counter of a cell.
  ///
  /// @param cell The cell index.
  ///
  /// @return The counter associated with *cell*.
  size_t count(size_t cell) const;

  /// Sets a cell to a given value.
  /// @param cell The cell whose value changes.
  /// @param value The new value of the cell.
  void set(size_t cell, size_t value);

  /// Sets all counter values to 0.
  void clear();

  /// Retrieves the number of cells.
  /// @return The number of cells in the counter vector.
  size_t size() const;

  /// Retrieves the maximum possible counter value.
  /// @return The maximum counter value constrained by the cell width.
  size_t max() const;

  /// Retrieves the counter width.
  /// @return The number of bits per cell.
  size_t width() const;

private:
  bitvector bits_;
  size_t width_;
};

/// Generates a string representation of a counter vector.
/// The arguments have the same meaning as in bf::bitvector.
std::string to_string(counter_vector const& v, bool all = false,
                      size_t cut_off = 0);

} // namespace bf

#endif
