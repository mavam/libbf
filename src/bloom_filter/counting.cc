#ifndef BF_BLOOM_FILTER_COUNTING_H
#define BF_BLOOM_FILTER_COUNTING_H

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
  : bloom_filter(std::move(h)),
    cells_(cells, width)
{
}

void counting_bloom_filter::add_impl(std::vector<digest> const& digests)
{
  for (auto d : digests)
    cells_.increment(d % bits_.size());
}

size_t counting_bloom_filter::lookup_impl(std::vector<digest> const& digests) const
{
  auto min = cells_.max();
  for (auto d : digests)
  {
    // TODO: use partitioning here to allow for reusing derived bounds of CMS.
    auto cnt = cells_[d % bits_.size()];
    if (cnt < min)
      return min = cnt;
  }
  return min;
}

void counting_bloom_filter::clear()
{
  cells_.clear();
}


void spectral_mi_bloom_filter::add_impl(std::vector<digest> const& digests)
{
  for (auto i : minima(cells_, digests))
    cells_.increment(i);
}


void spectral_rm_bloom_filter::add_impl(std::vector<digest> const& digests)
{
  counting_bloom_filter::add_impl(digests);

  auto mins = minima(cells_, digests);
  if (mins.size() == 1)
    // Do nothing if the minimum is recurring.
    return;

  // FIXME: hash again instead of resuing digests. This may mean we need to
  // pass in the object instead of just the digests and change the bloom filter
  // interface.
  mins = minima(second_, digests);
  auto min = minimum(second_, mins);
  if (min > 0)
    for (auto i : mins)
      second_.increment(i);
  else
    for (auto i : mins)
      second_.increment(i, min);
}

size_t spectral_rm_bloom_filter::lookup_impl(std::vector<digest> const& digests) const
{
  auto mins = minima(cells_, digests);
  if (mins.size() == 1)
    return cells_[mins[0]];

  // FIXME: see add_impl.
  auto min2 = minimum(second_, minima(second_, digests));
  if (min2 > 0)
    return min2;

  return cells_[mins[0]];
}

void spectral_rm_bloom_filter::clear()
{
  counting_bloom_filter::clear();
  second_.clear();
}

void spectral_rm_bloom_filter::remove(std::vector<digest> const& digests)
{
  assert(! "removal not yet implemented");
}

} // namespace bf
#endif
