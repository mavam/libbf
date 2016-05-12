#ifndef BF_ENDIAN_HPP
#define BF_ENDIAN_HPP

namespace bf {

/// Describes the endianess
enum class endian {
  native = __BYTE_ORDER__,
  little = __ORDER_LITTLE_ENDIAN__,
  big    = __ORDER_BIG_ENDIAN__
};

static_assert(endian::native == endian::little || endian::native == endian::big,
              "endian::native shall be one of endian::little or endian::big");

static_assert(endian::big != endian::little,
              "endian::big and endian::little shall have different values");

} // namespace bf

#endif // BF_ENDIAN_HPP
