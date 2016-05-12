#define SUITE hash_append
#include "test.hpp"

#include "bf/hash/hash_append.hpp"
#include "bf/hash/debug.hpp"
#include "bf/hash/fnv1a.hpp"

using namespace bf;

namespace {

struct foo {
  int x = 42;
  char y = '#';
};

template <class Hasher>
void hash_append(Hasher& h, foo const& f) {
  hash_append(h, f.x);
  hash_append(h, f.y);
}

} // namespace <anonymous>

TEST(hash_append) {
  uhash<fnv1a> h;
  std::cout << h(42) << std::endl;
}

TEST(custom type) {
  uhash<debug> h;
  foo x;
  CHECK_EQUAL(h(x), sizeof(int) + sizeof(char));
}
