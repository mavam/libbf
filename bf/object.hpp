#ifndef BF_OBJECT_HPP
#define BF_OBJECT_HPP

#include <type_traits>

namespace bf {

/// Wraps sequential data to be used in hashing.
class object
{
public:
  object(void const* data, size_t size)
    : data_(data), size_(size)
  {
  }

  void const* data() const
  {
    return data_;
  }

  size_t size() const
  {
    return size_;
  }

private:
  void const* data_ = nullptr;
  size_t size_ = 0;
};

} // namespace bf

#endif
