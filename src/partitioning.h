#ifndef BF_PARTITIONING
#define BF_PARTITIONING

namespace bf {

std::vector<size_t> partition(std::vector<digest> const& digests, size_t cells)
{
  assert(cells % digests.size() == 0);
  std::vector<size_t> indices(digests.size());
  size_t parts = cells / digests.size();
  for (size_t i = 0; i < indices.size(); ++i)
    indices[i] = digests[i] % parts + (i * parts);
  return indices;
};

std::vector<size_t> mod(std::vector<digest> const& digests, size_t cells)
{
  std::vector<size_t> indices(digests.size());
  for (size_t i = 0; i < indices.size(); ++i)
    indices[i] = digests[i] % cells;
  return indices;
};

} // namespace bf

#endif
