#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Bloom filter unit test"

#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include "bloom_filter/a2.h"
#include "bloom_filter/basic.h"
#include "bloom_filter/spectral.h"
#include "bloom_filter/stable.h"
#include "bloom_filter/bitwise.h"

using namespace bf;

BOOST_AUTO_TEST_CASE(store_fixed_width)
{
  core<>::store_type s(3, 2);

  BOOST_REQUIRE(s.increment(0));
  BOOST_REQUIRE_EQUAL(to_string(s), "100000");
  BOOST_REQUIRE_EQUAL(s.count(0), 1);

  BOOST_REQUIRE(s.increment(0));
  BOOST_REQUIRE_EQUAL(to_string(s), "010000");
  BOOST_REQUIRE_EQUAL(s.count(0), 2);

  BOOST_REQUIRE(s.increment(0));
  BOOST_REQUIRE_EQUAL(to_string(s), "110000");
  BOOST_REQUIRE_EQUAL(s.count(0), 3);

  BOOST_REQUIRE(! s.increment(0));
  BOOST_REQUIRE_EQUAL(to_string(s), "110000");
  BOOST_REQUIRE_EQUAL(s.count(0), 3);

  s = core<>::store_type(3, 32);
  auto max = std::numeric_limits<uint32_t>::max();
  BOOST_REQUIRE_EQUAL(s.max(), max);

  char value[64];
  decltype(max) step = 1 << 15;
  decltype(max) last = 0;
  for (decltype(max) i = 0; i < max && last <= i; i += step)
  {
    last = i;
    snprintf(value, sizeof(value), "%u", i);
    BOOST_REQUIRE_MESSAGE(s.count(0) == i, value);
    s.increment(0, step);
  }
}

BOOST_AUTO_TEST_CASE(store_fixed_width_increment)
{
  core<>::store_type s(2, 3);

  BOOST_REQUIRE(s.increment(0, 3));
  BOOST_REQUIRE(to_string(s) == "110000");
  BOOST_REQUIRE(s.count(0) == 3);

  BOOST_REQUIRE(s.increment(0, 1) == 1);
  BOOST_REQUIRE(to_string(s) == "001000");
  BOOST_REQUIRE(s.count(0) == 4);

  BOOST_REQUIRE(s.increment(0, 1));
  BOOST_REQUIRE(s.count(0) == 5);
  BOOST_REQUIRE(to_string(s) == "101000");
  BOOST_REQUIRE(! s.increment(0, 3));
  BOOST_REQUIRE(to_string(s) == "111000");

  BOOST_REQUIRE(s.increment(1, 4));
  BOOST_REQUIRE(to_string(s) == "111001");
  BOOST_REQUIRE(s.count(1) == 4);
  BOOST_REQUIRE(s.increment(1, 3));
  BOOST_REQUIRE(to_string(s) == "111111");
  BOOST_REQUIRE(! s.increment(1));
  BOOST_REQUIRE(! s.increment(1, 42));
}

BOOST_AUTO_TEST_CASE(exceptions)
{
  BOOST_CHECK_THROW(basic<>({1, 0, 1}), std::invalid_argument);
  BOOST_CHECK_THROW(basic<>({0, 1, 1}), std::invalid_argument);
  BOOST_CHECK_THROW(basic<>({1, 1, 0}), std::invalid_argument);
  BOOST_CHECK_THROW(basic<>({1, 1, 1, 0}), std::invalid_argument);
  BOOST_CHECK_THROW(basic<>({10, 1, 1, 3}), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(basic_bloom_filter)
{
  basic<> b({32, 5});

  b.add("foo");
  BOOST_CHECK(b.count("foo") == 1);

  b.add("bar");
  BOOST_CHECK(b.count("bar") == 1);

  b.add("baz");
  BOOST_CHECK(b.count("baz") == 1);

  b.add(4.2);
  BOOST_CHECK(b.count(4.2) == 1);

  b.add('c');
  BOOST_CHECK(b.count('c') == 1);

  b.add(4711ULL);
  BOOST_CHECK(b.count(4711ULL) == 1);

  BOOST_CHECK_EQUAL(b.count("qux"), 0);
  BOOST_CHECK_EQUAL(b.count("corge"), 1);    // False positive.
  BOOST_CHECK_EQUAL(b.count("graunt"), 0);
  BOOST_CHECK_EQUAL(b.count('a'), 0);
  BOOST_CHECK_EQUAL(b.count(3.1415), 0);
}

BOOST_AUTO_TEST_CASE(basic_bloom_filter_counting)
{
  typedef basic<>::core_type::store_type store;
  basic<core<store, double_hashing<>>> b({23, 3, 3});

  for (unsigned i = 0; i < 4; ++i)
  {
    b.add("qux");
    b.add("corge");
    b.add("grault");
    b.add(3.14159265);
  }

  BOOST_CHECK_EQUAL(b.count("qux"), 4);
  BOOST_CHECK_EQUAL(b.count("corge"), 4);
  BOOST_CHECK_EQUAL(b.count("grault"), 4);
  BOOST_CHECK_EQUAL(b.count(3.14159265), 4);
  BOOST_CHECK_EQUAL(b.count(3.14), 0);
  BOOST_CHECK_EQUAL(b.count("foo"), 0);

  for (unsigned i = 0; i < 4; ++i)
  {
    b.remove("grault");
    b.add("qux");
  }

  BOOST_CHECK_EQUAL(b.count("qux"), 7);
  BOOST_CHECK_EQUAL(b.count("corge"), 4);
}

BOOST_AUTO_TEST_CASE(stable_bloom_filter)
{
  stable<> b(3, {5, 3, 2});

  b.add("one fish");
  b.add("two fish");
  b.add("red fish");
  b.add("blue fish");

  BOOST_CHECK_EQUAL(to_string(b), "1101110011");
}

BOOST_AUTO_TEST_CASE(spectral_mi_bloom_filter)
{
  spectral_mi<> b({7, 3, 2});

  b.add("one fish");
  b.add("one fish"); // 0 2 0 0 2 0 2
  BOOST_CHECK_EQUAL(b.count("one fish"), 2);
  BOOST_CHECK_EQUAL(to_string(b), "00010000010001");

  b.add("blue fish"); // 0 2 1 0 2 0 2
  BOOST_CHECK_EQUAL(to_string(b), "00011000010001");

  b.add("blue fish"); // 0 2 2 0 2 0 2
  BOOST_CHECK_EQUAL(to_string(b), "00010100010001");
  BOOST_CHECK_EQUAL(b.count("blue fish"), 2);

  b.add("blue fish"); // 0 2 3 0 3 0 2
  BOOST_CHECK_EQUAL(to_string(b), "00011100110001");
  BOOST_CHECK_EQUAL(b.count("blue fish"), 3);
}

BOOST_AUTO_TEST_CASE(spectral_rm_bloom_filter)
{
  spectral_rm<> b({5, 2, 3}, {2, 2, 2});

  b.add("foo"); // 1 0 1 0 0 and 0 0
  BOOST_CHECK(b.count("foo") == 1);

  // For "bar", all hash functions return the same position, the we have
  // necessarily a recurring minimum (RM). Thus we do not look in the second
  // core and return 2, although the correct count would be 1.
  b.add("bar"); // 2 0 1 0 0 and 0 0
  BOOST_CHECK(b.count("bar") == 2);
  BOOST_CHECK(to_string(b) == "010000100000000\n0000");

  // For "foo", we encounter a unique minimum in the first core, but since
  // all positions for "foo" are zero in the second core, we return the
  // mimimum of the first, which is 1.
  BOOST_CHECK(b.count("foo") == 1);

  // After increasing the counters for "foo", we find that it (still) has a
  // unique minimum in in the first core. Hence we add its minimum to the
  // second core.
  b.add("foo"); // 3 0 2 0 0 and 2 2
  BOOST_CHECK(b.count("foo") == 2);
  BOOST_CHECK(to_string(b) == "110000010000000\n0101");

  // The "blue fish" causes some trouble: because its insertion yields a
  // unique minimum, we go into the second bitvector. There, we find that it
  // hashes to the same positions as foo, wich has a counter of 2. Because it
  // appears to exist there, we have to increment its counters. This falsely
  // bumps up the counter of "blue fish" to 3.
  b.add("blue fish"); // 3 0 3 0 1 and 3 3
  BOOST_CHECK(b.count("blue fish") == 3);
  BOOST_CHECK(to_string(b) == "110000110000100\n1111");

  // Since the "blue fish" has (still) a unique minimum after removing it one
  // time, we look in the second core and find it to be present there.
  // Hence we decrement the counters in the second core.
  b.remove("blue fish"); // 3 0 2 0 0 and 2 2
  BOOST_CHECK(b.count("blue fish") == 2);
  BOOST_CHECK(to_string(b) == "110000010000000\n0101");

  b.remove("blue fish");
  BOOST_CHECK(b.count("blue fish") == 1); // 3 0 1 0 0 and 1 1

  // Let's look at "foo". This fellow has now a unique minimum. Since it has
  // a unique minimum after the removal, we also decrement the counter in the
  // second core.
  b.remove("foo"); // 2 0 0 0 0 and 0 0
  BOOST_CHECK(b.count("foo") == 0);
  BOOST_CHECK(to_string(b) == "010000000000000\n0000");

  // Alas, we violated Claim 1 in Section 2.2 in the paper! The spectral
  // Bloom filter returns a count of 0 for "foo", although it should be 1.
  // Thus, the frequency estimate is no longer a lower bound. This occurs
  // presumably due to the fact that we remove "blue fish" twice although we
  // added it only once.
}

BOOST_AUTO_TEST_CASE(bitwise_bloom_filter)
{
  bitwise<> b({4, 3, 1}, 1);

  b.add("foo");
  BOOST_CHECK(b.count("foo") == 1);
  BOOST_CHECK(to_string(b) == "0011");

  b.add("foo");
  BOOST_CHECK(b.count("foo") == 2);
  BOOST_CHECK(to_string(b) == "11\n0000");

  for (unsigned i = 3; i < 9; ++i)
  {
      b.add("foo");
      BOOST_CHECK(b.count("foo") == i);
  }

  BOOST_CHECK(to_string(b) == "1\n0\n00\n0000");
  BOOST_CHECK(b.count("foo") == 8);

  b.remove("foo");
  BOOST_CHECK(b.count("foo") == 7);
  BOOST_CHECK(to_string(b) == "1\n11\n0011");

  for (unsigned i = 7; i > 1; --i)
  {
      BOOST_CHECK(b.count("foo") == i);
      b.remove("foo");
  }

  BOOST_CHECK(b.count("foo") == 1);
  BOOST_CHECK(to_string(b) == "0011");

  b.remove("foo");
  b.remove("foo");
  BOOST_CHECK(b.count("foo") == 0);
  BOOST_CHECK(to_string(b) == "0000");
}

// Test with a core that has more than one bit of cell width.
BOOST_AUTO_TEST_CASE(bitwise_bloom_filter2)
{
  bitwise<> b({4, 3, 2}, 1);

  b.add("foo");
  BOOST_CHECK(b.count("foo") == 1);
  BOOST_CHECK(to_string(b) == "00001010");

  b.add("foo");
  BOOST_CHECK(b.count("foo") == 2);
  BOOST_CHECK(to_string(b) == "00000101");

  b.add("foo");
  BOOST_CHECK(b.count("foo") == 3);
  BOOST_CHECK(to_string(b) == "00001111");

  b.add("foo");
  BOOST_CHECK(b.count("foo") == 4);
  BOOST_CHECK(to_string(b) == "1010\n00000000");

  b.remove("foo");
  BOOST_CHECK(b.count("foo") == 3);
  b.remove("foo");
  BOOST_CHECK(b.count("foo") == 2);
  b.remove("foo");
  BOOST_CHECK(b.count("foo") == 1);
  BOOST_CHECK(to_string(b) == "00001010");
}
BOOST_AUTO_TEST_CASE(a2_bloom_filter)
{
  a2<> b({5, 2, 1}, 1);

  b.add("foo");
  b.add("foo");
  BOOST_CHECK(b.count("foo") == 1);
  BOOST_CHECK(to_string(b) == "10100\n00000");

  BOOST_CHECK(b.count("bar") == 1);   // FP
  b.add("baz"); // Causes core swapping.
  BOOST_CHECK(to_string(b) == "01000\n11100");
  b.add("qux"); // Swap again.
  BOOST_CHECK(to_string(b) == "00001\n01001");
}
