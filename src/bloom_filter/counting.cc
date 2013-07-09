#include "bloom_filter/counting.h"

#include <cassert>
#include "partitioning.h"

namespace bf {
namespace {

/// Computes minima for a vector and a list of indices.
/// @param cv A counter vector instance.
/// @param ind The indices to use when looking for minima.
std::vector<size_t> minima(counter_vector const& cv,
                           std::vector<size_t> const& ind)
{
  auto min = cv.max();
  std::vector<size_t> positions;
  for (auto i : ind)
  {
    auto cnt = cv.count(i);
    if (cnt == min)
    {
      positions.push_back(i);
    }
    else if (cnt < min)
    {
      min = cnt;
      positions.clear();
      positions.push_back(i);
    }
  }
  return positions;
}
} // namespace <anonymous>


counting_bloom_filter::counting_bloom_filter(hasher h, size_t cells,
                                             size_t width)
  : hasher_(std::move(h)),
    cells_(cells, width)
{
}

void counting_bloom_filter::add(object const& o)
{
  for (auto d : hasher_(o))
    cells_.increment(d % cells_.size());
}

size_t counting_bloom_filter::lookup(object const& o) const
{
  auto min = cells_.max();
  for (auto i : partition(hasher_(o), cells_.size()))
  {
    auto cnt = cells_.count(i);
    if (cnt < min)
      return min = cnt;
  }
  return min;
}

void counting_bloom_filter::clear()
{
  cells_.clear();
}


spectral_mi_bloom_filter::spectral_mi_bloom_filter(hasher h, size_t cells,
                                                   size_t width)
  : counting_bloom_filter(std::move(h), cells, width)
{
}

void spectral_mi_bloom_filter::add(object const& o)
{
  for (auto i : minima(cells_, mod(hasher_(o), cells_.size())))
    cells_.increment(i);
}


spectral_rm_bloom_filter::spectral_rm_bloom_filter(
    hasher h1, size_t cells1, size_t width1,
    hasher h2, size_t cells2, size_t width2)
  : counting_bloom_filter(std::move(h1), cells1, width1),
    hasher2_(std::move(h2)),
    cells2_(cells2, width2)
{
}

// "When adding an item x, increase the counters of x in the primary SBF. Then
// check if x has a recurring minimum. If so, continue normally. Otherwise (if
// x has a single minimum), look for x in the secondary SBF. If found, increase
// its counters, otherwise add x to the secondary SBF, with an initial value
// that equals its minimal value from the primary SBF."
void spectral_rm_bloom_filter::add(object const& o)
{
  auto digests = hasher_(o);
  auto indices = mod(digests, cells_.size());
  for (auto i : indices)
    cells_.increment(i);

  auto mins = minima(cells_, indices);
  if (mins.size() > 1)
    return;

  assert(mins.size() == 1);
  auto min1 = cells_.count(mins[0]);
  indices = mod(hasher2_(o), cells2_.size());
  mins = minima(cells2_, indices);
  auto min2 = cells2_.count(mins[0]);
  if (min2 > 0)
    // Note: it's unclear to me whether "increase its counters" means increase
    // only the minima or all indices. I opted for the latter (same during
    // deletion).
    for (auto i : indices)
      cells2_.increment(i);
  else
    for (auto i : indices)
      cells2_.increment(i, min1);
}

// "When performing lookup for x, check if x has a recurring minimum in the
// primary SBF. If so return the minimum. Otherwise, perform lookup for x in
// secondary SBF. If [the] returned value is greater than 0, return it.
// Otherwise, return minimum from primary SBF."
size_t spectral_rm_bloom_filter::lookup(object const& o) const
{
  auto mins1 = minima(cells_, mod(hasher_(o), cells_.size()));
  auto min1 = cells_.count(mins1[0]);
  if (mins1.size() > 1)
    return min1;
  auto mins2 = minima(cells2_, mod(hasher2_(o), cells2_.size()));
  auto min2 = cells2_.count(mins2[0]);
  return min2 > 0 ? min2 : min1;
}

void spectral_rm_bloom_filter::clear()
{
  cells_.clear();
  cells2_.clear();
}

// "First decrease its counters in the primary SBF, then if it has a single
// minimum (or if it exists in Bf) decrease its counters in the secondary SBF,
// unless at least one of them is 0."
void spectral_rm_bloom_filter::remove(object const& o)
{
  auto digests = hasher_(o);
  auto indices = mod(digests, cells_.size());
  for (auto i : indices)
    cells_.decrement(i);

  auto mins = minima(cells_, indices);
  if (mins.size() > 1)
    return;

  indices = mod(hasher2_(o), cells2_.size());
  mins = minima(cells2_, indices);
  if (cells2_.count(mins[0]) > 0)
    for (auto i : indices)
      cells_.decrement(i);
}

} // namespace bf
