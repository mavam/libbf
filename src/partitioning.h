#ifndef BF_PARTITIONING
#define BF_PARTITIONING

namespace bf {

/// Maps a vector of digests into *k* partitions of the available cells.
/// @param digests The *k* digests to partition
/// @param cells The number cells to digest the *k* digests into.
/// @return The indices corresponding to *digests*.
std::vector<size_t> partition(std::vector<digest> const& digests, size_t cells)
{
  assert(cells % digests.size() == 0);
  std::vector<size_t> indices(digests.size());
  size_t parts = cells / digests.size();
  for (size_t i = 0; i < indices.size(); ++i)
    indices[i] = digests[i] % parts + (i * parts);
  return indices;
};

/// Maps a vector of digests uniformly to indices using modulus.
/// @param digests The *k* digests to partition
/// @param cells The number cells to digest the *k* digests into.
/// @return The indices corresponding to *digests*.
std::vector<size_t> mod(std::vector<digest> const& digests, size_t cells)
{
  std::vector<size_t> indices(digests.size());
  for (size_t i = 0; i < indices.size(); ++i)
    indices[i] = digests[i] % cells;
  return indices;
};

} // namespace bf

#endif
