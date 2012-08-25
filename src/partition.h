#ifndef BF_PARTITION_H
#define BF_PARTITION_H

namespace bf {

/// The base class for partition policies.
/// \tparam The type of the derived policy.
template <typename Derived>
class partition_policy
{
public:
  unsigned parts() const
  {
    return derived().parts();
  }

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

/// Do not perform partitioning; use the whole bitvector for all \f$k\f$ hash
/// values.
class no_partitioning : public partition_policy<no_partitioning>
{
public:
  no_partitioning(unsigned) = default;

  unsigned parts() const
  {
    return 1;
  }

  unsigned position(std::size_t hash, unsigned i, unsigned cells) const
  {
    return hash % cells;
  }
};

/// Partition the bitvector of size \f$m\f$ in \f$k\f$ sub-vectors of size
/// \f$\frac{m}{k}\f$. Each hash value \f$h_i\f$ for \f$i\in[k]\f$ maps to one
/// of the sub-vectors.
struct partitioning : public partition_policy<partitioning>
{
public:
  partitioning(unsigned parts)
    : parts_(parts)
  {
  }

  unsigned parts() const
  {
    return parts_;
  }

  unsigned position(std::size_t hash, unsigned i, unsigned cells) const
  {
    assert(cells % parts_ == 0);
    auto part_size = cells / parts_;
    return hash % part_size + (i * part_size);
  }

private:
  unsigned parts_;
};

} // namespace partition

#endif
