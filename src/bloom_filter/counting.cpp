#include <bf/bloom_filter/counting.hpp>

#include <algorithm>
#include <cassert>

namespace bf {

counting_bloom_filter::counting_bloom_filter(hasher h, size_t cells,
                                             size_t width, bool partition)
    : hasher_(std::move(h)), cells_(cells, width), partition_(partition) {
}

void counting_bloom_filter::add(object const& o) {
  increment(find_indices(o));
}

size_t counting_bloom_filter::lookup(object const& o) const {
  auto min = cells_.max();
  for (auto i : find_indices(o)) {
    auto cnt = cells_.count(i);
    if (cnt < min)
      return min = cnt;
  }
  return min;
}

void counting_bloom_filter::clear() {
  cells_.clear();
}

void counting_bloom_filter::remove(object const& o) {
  decrement(find_indices(o));
}

std::vector<size_t> counting_bloom_filter::find_indices(object const& o) const {
  auto digests = hasher_(o);
  std::vector<size_t> indices(digests.size());
  if (partition_) {
    assert(cells_.size() % digests.size() == 0);
    auto const parts = cells_.size() / digests.size();
    for (size_t i = 0; i < indices.size(); ++i)
      indices[i] = (i * parts) + digests[i] % parts;
  } else {
    for (size_t i = 0; i < indices.size(); ++i)
      indices[i] = digests[i] % cells_.size();
  }
  std::sort(indices.begin(), indices.end());
  indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
  return indices;
};

size_t
counting_bloom_filter::find_minimum(std::vector<size_t> const& indices) const {
  auto min = cells_.max();
  for (auto i : indices) {
    auto cnt = cells_.count(i);
    if (cnt < min)
      min = cnt;
  }
  return min;
}

std::vector<size_t>
counting_bloom_filter::find_minima(std::vector<size_t> const& indices) const {
  auto min = cells_.max();
  std::vector<size_t> positions;
  for (auto i : indices) {
    auto cnt = cells_.count(i);
    if (cnt == min) {
      positions.push_back(i);
    } else if (cnt < min) {
      min = cnt;
      positions.clear();
      positions.push_back(i);
    }
  }
  return positions;
}

bool counting_bloom_filter::increment(std::vector<size_t> const& indices,
                                      size_t value) {
  auto status = true;
  for (auto i : indices)
    if (!cells_.increment(i, value))
      status = false;
  return status;
}

bool counting_bloom_filter::decrement(std::vector<size_t> const& indices,
                                      size_t value) {
  auto status = true;
  for (auto i : indices)
    if (!cells_.decrement(i, value))
      status = false;
  return status;
}

size_t counting_bloom_filter::count(size_t index) const {
  return cells_.count(index);
}

spectral_mi_bloom_filter::spectral_mi_bloom_filter(hasher h, size_t cells,
                                                   size_t width, bool partition)
    : counting_bloom_filter(std::move(h), cells, width, partition) {
}

void spectral_mi_bloom_filter::add(object const& o) {
  increment(find_minima(find_indices(o)));
}

spectral_rm_bloom_filter::spectral_rm_bloom_filter(hasher h1, size_t cells1,
                                                   size_t width1, hasher h2,
                                                   size_t cells2, size_t width2,
                                                   bool partition)
    : first_(std::move(h1), cells1, width1, partition),
      second_(std::move(h2), cells2, width2, partition) {
}

// "When adding an item x, increase the counters of x in the primary SBF. Then
// check if x has a recurring minimum. If so, continue normally. Otherwise (if
// x has a single minimum), look for x in the secondary SBF. If found, increase
// its counters, otherwise add x to the secondary SBF, with an initial value
// that equals its minimal value from the primary SBF."
void spectral_rm_bloom_filter::add(object const& o) {
  auto indices1 = first_.find_indices(o);
  first_.increment(indices1);
  auto mins1 = first_.find_minima(indices1);
  if (mins1.size() > 1)
    return;

  auto indices2 = second_.find_indices(o);
  auto min1 = first_.count(mins1[0]);
  auto min2 = second_.find_minimum(indices2);

  // Note: it's unclear to me whether "increase its counters" means increase
  // only the minima or all indices. I opted for the latter (same during
  // deletion).
  second_.increment(indices2, min2 > 0 ? 1 : min1);
}

// "When performing lookup for x, check if x has a recurring minimum in the
// primary SBF. If so return the minimum. Otherwise, perform lookup for x in
// secondary SBF. If [the] returned value is greater than 0, return it.
// Otherwise, return minimum from primary SBF."
size_t spectral_rm_bloom_filter::lookup(object const& o) const {
  auto mins1 = first_.find_minima(first_.find_indices(o));
  auto min1 = first_.count(mins1[0]);
  if (mins1.size() > 1)
    return min1;
  auto min2 = second_.find_minimum(second_.find_indices(o));
  return min2 > 0 ? min2 : min1;
}

void spectral_rm_bloom_filter::clear() {
  first_.clear();
  second_.clear();
}

// "First decrease its counters in the primary SBF, then if it has a single
// minimum (or if it exists in Bf) decrease its counters in the secondary SBF,
// unless at least one of them is 0."
void spectral_rm_bloom_filter::remove(object const& o) {
  auto indices1 = first_.find_indices(o);
  first_.decrement(indices1);
  auto mins1 = first_.find_minima(indices1);
  if (mins1.size() > 1)
    return;

  auto indices2 = second_.find_indices(o);
  if (second_.find_minimum(indices2) > 0)
    second_.decrement(indices2);
}

} // namespace bf
