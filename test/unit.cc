#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Bloom filter unit test"

#include <iostream>
#include "test.h"
#include "counter_vector.h"
#include "bloom_filter/a2.h"
#include "bloom_filter/basic.h"
#include "bloom_filter/counting.h"
#include "bloom_filter/stable.h"
#include "bloom_filter/bitwise.h"

using namespace bf;

BOOST_AUTO_TEST_CASE(counter_vector_incrementing)
{
  counter_vector v(3, 2);

  BOOST_CHECK(v.increment(0));
  BOOST_CHECK_EQUAL(to_string(v), "100000");
  BOOST_CHECK_EQUAL(v.count(0), 1);

  BOOST_CHECK(v.increment(0));
  BOOST_CHECK_EQUAL(to_string(v), "010000");
  BOOST_CHECK_EQUAL(v.count(0), 2);

  BOOST_CHECK(v.increment(0));
  BOOST_CHECK_EQUAL(to_string(v), "110000");
  BOOST_CHECK_EQUAL(v.count(0), 3);

  // Already reached maximum counter value with 2 bits.
  BOOST_CHECK(! v.increment(0));
  BOOST_CHECK_EQUAL(to_string(v), "110000");
  BOOST_CHECK_EQUAL(v.count(0), 3);

  BOOST_CHECK(v.increment(1));
  BOOST_CHECK_EQUAL(to_string(v), "111000");
  BOOST_CHECK_EQUAL(v.count(1), 1);

  BOOST_CHECK(v.increment(2));
  BOOST_CHECK_EQUAL(to_string(v), "111010");
  BOOST_CHECK_EQUAL(v.count(2), 1);
}

BOOST_AUTO_TEST_CASE(counter_vector_adding)
{
  counter_vector v(2, 3);

  BOOST_CHECK(v.increment(0, 3));
  BOOST_CHECK_EQUAL(to_string(v), "110000");
  BOOST_CHECK_EQUAL(v.count(0), 3);

  BOOST_CHECK(v.increment(0, 1) == 1);
  BOOST_CHECK_EQUAL(to_string(v), "001000");
  BOOST_CHECK_EQUAL(v.count(0), 4);

  BOOST_CHECK(v.increment(0, 1));
  BOOST_CHECK_EQUAL(v.count(0), 5);
  BOOST_CHECK_EQUAL(to_string(v), "101000");

  BOOST_CHECK(! v.increment(0, 3));
  BOOST_CHECK_EQUAL(to_string(v), "111000");
  BOOST_CHECK_EQUAL(v.count(0), 7);

  BOOST_CHECK(v.increment(1, 4));
  BOOST_CHECK_EQUAL(to_string(v), "111001");
  BOOST_CHECK_EQUAL(v.count(1), 4);

  BOOST_CHECK(v.increment(1, 3));
  BOOST_CHECK_EQUAL(to_string(v), "111111");
  BOOST_CHECK(! v.increment(1));
  BOOST_CHECK(! v.increment(1, 42));
}

BOOST_AUTO_TEST_CASE(counter_vector_adding_big)
{
  counter_vector v(3, 32);
  auto max = std::numeric_limits<uint32_t>::max();
  BOOST_REQUIRE_EQUAL(v.max(), max);

  std::string value;
  size_t step = 1 << 15;
  size_t last = 0;
  for (size_t i = 0; i < max && last <= i; i += step)
  {
    last = i;
    value = std::to_string(i);
    BOOST_REQUIRE_MESSAGE(v.count(0) == i, value);
    v.increment(0, step);
  }
}

BOOST_AUTO_TEST_CASE(bloom_filter_basic)
{
  basic_bloom_filter bf(0.8, 10);

  bf.add("foo");
  bf.add("bar");
  bf.add("baz");
  bf.add('c');
  bf.add(4.2);
  bf.add(4711ULL);

  // True-positives
  BOOST_CHECK_EQUAL(bf.lookup("foo"), 1);
  BOOST_CHECK_EQUAL(bf.lookup("bar"), 1);
  BOOST_CHECK_EQUAL(bf.lookup("baz"), 1);
  BOOST_CHECK_EQUAL(bf.lookup(4.2), 1);
  BOOST_CHECK_EQUAL(bf.lookup('c'), 1);
  BOOST_CHECK_EQUAL(bf.lookup(4711ULL), 1);

  // True-negatives
  BOOST_CHECK_EQUAL(bf.lookup("qux"), 0);
  BOOST_CHECK_EQUAL(bf.lookup("graunt"), 0);
  BOOST_CHECK_EQUAL(bf.lookup(3.1415), 0);

  // False-positives
  BOOST_CHECK_EQUAL(bf.lookup("corge"), 1);
  BOOST_CHECK_EQUAL(bf.lookup('a'), 1);
}

BOOST_AUTO_TEST_CASE(bloom_filter_counting)
{
  counting_bloom_filter bf(make_hasher(3), 10, 2);

  for (size_t i = 0; i < 3; ++i)
  {
    bf.add("qux");
    bf.add("corge");
    bf.add("grault");
    bf.add(3.14159265);
  }

  BOOST_CHECK_EQUAL(bf.lookup("foo"), 0);
  BOOST_CHECK_EQUAL(bf.lookup("qux"), 3);
  BOOST_CHECK_EQUAL(bf.lookup("corge"), 3);
  BOOST_CHECK_EQUAL(bf.lookup("grault"), 3);
  BOOST_CHECK_EQUAL(bf.lookup(3.14159265), 3);

  for (size_t i = 0; i < 3; ++i)
    bf.remove("grault");
  BOOST_CHECK_EQUAL(bf.lookup("corge"), 0);
}

BOOST_AUTO_TEST_CASE(bloom_filter_spectral_mi)
{
  spectral_mi_bloom_filter bf(make_hasher(3), 8, 2);

  bf.add("oh");
  bf.add("oh");
  bf.add("my");
  bf.add("god");
  bf.add("becky");
  bf.add("look");
  BOOST_CHECK_EQUAL(bf.lookup("oh"), 2);
  BOOST_CHECK_EQUAL(bf.lookup("my"), 1);
  BOOST_CHECK_EQUAL(bf.lookup("god"), 1);
  BOOST_CHECK_EQUAL(bf.lookup("becky"), 1);
  BOOST_CHECK_EQUAL(bf.lookup("look"), 2); // FP, same cells as "god".
}

BOOST_AUTO_TEST_CASE(bloom_filter_spectral_rm)
{
  auto h1 = make_hasher(3, 0);
  auto h2 = make_hasher(3, 1);
  spectral_rm_bloom_filter bf(std::move(h1), 5, 2, std::move(h2), 4, 2);

  bf.add("foo");
  BOOST_CHECK_EQUAL(bf.lookup("foo"), 1);

  // TODO: port old unit tests and double-check the implementation.

  //// For "bar", all hash functions return the same position, the we have
  //// necessarily a recurring minimum (RM). Thus we do not look in the second
  //// core and return 2, although the correct count would be 1.
  //b.add("bar"); // 2 0 1 0 0 and 0 0
  //BOOST_CHECK(b.count("bar") == 2);
  //BOOST_CHECK(to_string(b) == "010000100000000\n0000");

  //// For "foo", we encounter a unique minimum in the first core, but since
  //// all positions for "foo" are zero in the second core, we return the
  //// mimimum of the first, which is 1.
  //BOOST_CHECK(b.count("foo") == 1);

  //// After increasing the counters for "foo", we find that it (still) has a
  //// unique minimum in in the first core. Hence we add its minimum to the
  //// second core.
  //b.add("foo"); // 3 0 2 0 0 and 2 2
  //BOOST_CHECK(b.count("foo") == 2);
  //BOOST_CHECK(to_string(b) == "110000010000000\n0101");

  //// The "blue fish" causes some trouble: because its insertion yields a
  //// unique minimum, we go into the second bitvector. There, we find that it
  //// hashes to the same positions as foo, wich has a counter of 2. Because it
  //// appears to exist there, we have to increment its counters. This falsely
  //// bumps up the counter of "blue fish" to 3.
  //b.add("blue fish"); // 3 0 3 0 1 and 3 3
  //BOOST_CHECK(b.count("blue fish") == 3);
  //BOOST_CHECK(to_string(b) == "110000110000100\n1111");

  //// Since the "blue fish" has (still) a unique minimum after removing it one
  //// time, we look in the second core and find it to be present there.
  //// Hence we decrement the counters in the second core.
  //b.remove("blue fish"); // 3 0 2 0 0 and 2 2
  //BOOST_CHECK(b.count("blue fish") == 2);
  //BOOST_CHECK(to_string(b) == "110000010000000\n0101");

  //b.remove("blue fish");
  //BOOST_CHECK(b.count("blue fish") == 1); // 3 0 1 0 0 and 1 1

  //// Let's look at "foo". This fellow has now a unique minimum. Since it has
  //// a unique minimum after the removal, we also decrement the counter in the
  //// second core.
  //b.remove("foo"); // 2 0 0 0 0 and 0 0
  //BOOST_CHECK(b.count("foo") == 0);
  //BOOST_CHECK(to_string(b) == "010000000000000\n0000");

  //// Alas, we violated Claim 1 in Section 2.2 in the paper! The spectral
  //// Bloom filter returns a count of 0 for "foo", although it should be 1.
  //// Thus, the frequency estimate is no longer a lower bound. This occurs
  //// presumably due to the fact that we remove "blue fish" twice although we
  //// added it only once.
}

BOOST_AUTO_TEST_CASE(bloom_filter_bitwise)
{
  bitwise_bloom_filter bf(3, 8);

  BOOST_CHECK_EQUAL(bf.lookup("foo"), 0);
  bf.add("foo");
  BOOST_CHECK_EQUAL(bf.lookup("foo"), 1);
  bf.add("foo");
  BOOST_CHECK_EQUAL(bf.lookup("foo"), 2);
  bf.add("foo");
  BOOST_CHECK_EQUAL(bf.lookup("foo"), 3);

  BOOST_CHECK_EQUAL(bf.lookup("baz"), 0);
  bf.add("baz");
  BOOST_CHECK_EQUAL(bf.lookup("baz"), 1);
  BOOST_CHECK_EQUAL(bf.lookup("foo"), 3);
  bf.add("baz");
  BOOST_CHECK_EQUAL(bf.lookup("baz"), 2);
  BOOST_CHECK_EQUAL(bf.lookup("foo"), 3);
}

BOOST_AUTO_TEST_CASE(bloom_filter_stable)
{
  stable_bloom_filter bf(make_hasher(3), 11, 2, 2);

  bf.add("one fish");
  bf.add("two fish");
  bf.add("red fish");
  bf.add("blue fish");
  bf.add("green fish");
  bf.add("cyan fish");
  bf.add("yellow fish");
  bf.add("orange fish");
  bf.add("purple fish");
  bf.add("pink fish");
  bf.add("brown fish");
  bf.add("white fish");
  bf.add("black fish");
  bf.add("grey fish");
  bf.add("jelly fish");

  BOOST_CHECK_EQUAL(bf.lookup("one fish"), 0);
  BOOST_CHECK_EQUAL(bf.lookup("two fish"), 2);
  BOOST_CHECK_EQUAL(bf.lookup("red fish"), 3);
  BOOST_CHECK_EQUAL(bf.lookup("blue fish"), 3);
}

BOOST_AUTO_TEST_CASE(bloom_filter_a2)
{
  a2_bloom_filter bf(3, 32, 3);

  bf.add("foo");
  bf.add("foo");  // Duplicate inserts have no effect.
  bf.add("bar");
  bf.add("baz");

  // Reaches capacity and causes swapping.
  bf.add("qux");

  BOOST_CHECK_EQUAL(bf.lookup("foo"), 1);
  BOOST_CHECK_EQUAL(bf.lookup("bar"), 1);
  BOOST_CHECK_EQUAL(bf.lookup("baz"), 1);
  BOOST_CHECK_EQUAL(bf.lookup("qux"), 1);
}

