#include "bloom_filter/counting.h"

#include <cassert>

namespace bf {

namespace {

// TODO: factor this function so that it takes positions instead of digests in
// order to become independent of the partitioning policy.
std::vector<size_t> minima(counter_vector const& v,
                           std::vector<digest> const& d)
{
  auto min = v.max();
  std::vector<size_t> positions;
  for (auto i : d)
  {
    auto cnt = v.count(i % v.size()); // TODO: use partitioning
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
  for (auto d : hasher_(o))
  {
    // TODO: use partitioning here to allow for reusing derived bounds of CMS.
    auto cnt = cells_.count(d % cells_.size());
    if (cnt < min)
      return min = cnt;
  }
  return min;
}

void counting_bloom_filter::clear()
{
  cells_.clear();
}


void spectral_mi_bloom_filter::add(object const& o)
{
  for (auto i : minima(cells_, hasher_(o)))
    cells_.increment(i);
}


void spectral_rm_bloom_filter::add(object const& o)
{
  auto digests = hasher_(o);
  for (auto d : digests)
    cells_.increment(d % cells_.size());

  auto mins = minima(cells_, digests);
  if (mins.size() == 1)
    // Do nothing if the minimum is recurring.
    return;

  auto digests2 = hasher2_(o);
  mins = minima(cells2_, digests);
  // TODO
  //auto min = minimum(cells2_, mins);
  //if (min > 0)
  //  for (auto i : mins)
  //    cells2_.increment(i);
  //else
  //  for (auto i : mins)
  //    cells2_.increment(i, min);
}

size_t spectral_rm_bloom_filter::lookup(object const& o) const
{
  auto digests = hasher_(o);
  auto mins = minima(cells_, digests);
  if (mins.size() == 1)
    return cells_.count(mins[0]);

  auto digests2 = hasher2_(o);
  // TODO
  //auto min2 = minimum(cells2_, minima(cells2_, digests));
  //if (min2 > 0)
  //  return min2;

  return cells_.count(mins[0]);
}

void spectral_rm_bloom_filter::clear()
{
  counting_bloom_filter::clear();
  cells2_.clear();
}

void spectral_rm_bloom_filter::remove(object const& o)
{
  assert(! "removal not yet implemented");
}

} // namespace bf
