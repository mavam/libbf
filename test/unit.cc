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

BOOST_AUTO_TEST_CASE(bloom_filter_stable)
{
  stable_bloom_filter bf(make_hasher(3), 10, 2, 2);

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

  BOOST_CHECK_EQUAL(bf.lookup("one fish"), 2);
  BOOST_CHECK_EQUAL(bf.lookup("two fish"), 1);
  BOOST_CHECK_EQUAL(bf.lookup("red fish"), 3);
  BOOST_CHECK_EQUAL(bf.lookup("blue fish"), 0);
}

//BOOST_AUTO_TEST_CASE(spectral_mi_bloom_filter)
//{
//  spectral_mi<> b({7, 3, 2});
//
//  b.add("one fish");
//  b.add("one fish"); // 0 2 0 0 2 0 2
//  BOOST_CHECK_EQUAL(b.count("one fish"), 2);
//  BOOST_CHECK_EQUAL(to_string(b), "00010000010001");
//
//  b.add("blue fish"); // 0 2 1 0 2 0 2
//  BOOST_CHECK_EQUAL(to_string(b), "00011000010001");
//
//  b.add("blue fish"); // 0 2 2 0 2 0 2
//  BOOST_CHECK_EQUAL(to_string(b), "00010100010001");
//  BOOST_CHECK_EQUAL(b.count("blue fish"), 2);
//
//  b.add("blue fish"); // 0 2 3 0 3 0 2
//  BOOST_CHECK_EQUAL(to_string(b), "00011100110001");
//  BOOST_CHECK_EQUAL(b.count("blue fish"), 3);
//}
//
//BOOST_AUTO_TEST_CASE(spectral_rm_bloom_filter)
//{
//  spectral_rm<> b({5, 2, 3}, {2, 2, 2});
//
//  b.add("foo"); // 1 0 1 0 0 and 0 0
//  BOOST_CHECK(b.count("foo") == 1);
//
//  // For "bar", all hash functions return the same position, the we have
//  // necessarily a recurring minimum (RM). Thus we do not look in the second
//  // core and return 2, although the correct count would be 1.
//  b.add("bar"); // 2 0 1 0 0 and 0 0
//  BOOST_CHECK(b.count("bar") == 2);
//  BOOST_CHECK(to_string(b) == "010000100000000\n0000");
//
//  // For "foo", we encounter a unique minimum in the first core, but since
//  // all positions for "foo" are zero in the second core, we return the
//  // mimimum of the first, which is 1.
//  BOOST_CHECK(b.count("foo") == 1);
//
//  // After increasing the counters for "foo", we find that it (still) has a
//  // unique minimum in in the first core. Hence we add its minimum to the
//  // second core.
//  b.add("foo"); // 3 0 2 0 0 and 2 2
//  BOOST_CHECK(b.count("foo") == 2);
//  BOOST_CHECK(to_string(b) == "110000010000000\n0101");
//
//  // The "blue fish" causes some trouble: because its insertion yields a
//  // unique minimum, we go into the second bitvector. There, we find that it
//  // hashes to the same positions as foo, wich has a counter of 2. Because it
//  // appears to exist there, we have to increment its counters. This falsely
//  // bumps up the counter of "blue fish" to 3.
//  b.add("blue fish"); // 3 0 3 0 1 and 3 3
//  BOOST_CHECK(b.count("blue fish") == 3);
//  BOOST_CHECK(to_string(b) == "110000110000100\n1111");
//
//  // Since the "blue fish" has (still) a unique minimum after removing it one
//  // time, we look in the second core and find it to be present there.
//  // Hence we decrement the counters in the second core.
//  b.remove("blue fish"); // 3 0 2 0 0 and 2 2
//  BOOST_CHECK(b.count("blue fish") == 2);
//  BOOST_CHECK(to_string(b) == "110000010000000\n0101");
//
//  b.remove("blue fish");
//  BOOST_CHECK(b.count("blue fish") == 1); // 3 0 1 0 0 and 1 1
//
//  // Let's look at "foo". This fellow has now a unique minimum. Since it has
//  // a unique minimum after the removal, we also decrement the counter in the
//  // second core.
//  b.remove("foo"); // 2 0 0 0 0 and 0 0
//  BOOST_CHECK(b.count("foo") == 0);
//  BOOST_CHECK(to_string(b) == "010000000000000\n0000");
//
//  // Alas, we violated Claim 1 in Section 2.2 in the paper! The spectral
//  // Bloom filter returns a count of 0 for "foo", although it should be 1.
//  // Thus, the frequency estimate is no longer a lower bound. This occurs
//  // presumably due to the fact that we remove "blue fish" twice although we
//  // added it only once.
//}
//
//BOOST_AUTO_TEST_CASE(bitwise_bloom_filter)
//{
//  bitwise<> b({4, 3, 1}, 1);
//
//  b.add("foo");
//  BOOST_CHECK(b.count("foo") == 1);
//  BOOST_CHECK(to_string(b) == "0011");
//
//  b.add("foo");
//  BOOST_CHECK(b.count("foo") == 2);
//  BOOST_CHECK(to_string(b) == "11\n0000");
//
//  for (unsigned i = 3; i < 9; ++i)
//  {
//      b.add("foo");
//      BOOST_CHECK(b.count("foo") == i);
//  }
//
//  BOOST_CHECK(to_string(b) == "1\n0\n00\n0000");
//  BOOST_CHECK(b.count("foo") == 8);
//
//  b.remove("foo");
//  BOOST_CHECK(b.count("foo") == 7);
//  BOOST_CHECK(to_string(b) == "1\n11\n0011");
//
//  for (unsigned i = 7; i > 1; --i)
//  {
//      BOOST_CHECK(b.count("foo") == i);
//      b.remove("foo");
//  }
//
//  BOOST_CHECK(b.count("foo") == 1);
//  BOOST_CHECK(to_string(b) == "0011");
//
//  b.remove("foo");
//  b.remove("foo");
//  BOOST_CHECK(b.count("foo") == 0);
//  BOOST_CHECK(to_string(b) == "0000");
//}
//
//// Test with a core that has more than one bit of cell width.
//BOOST_AUTO_TEST_CASE(bitwise_bloom_filter2)
//{
//  bitwise<> b({4, 3, 2}, 1);
//
//  b.add("foo");
//  BOOST_CHECK(b.count("foo") == 1);
//  BOOST_CHECK(to_string(b) == "00001010");
//
//  b.add("foo");
//  BOOST_CHECK(b.count("foo") == 2);
//  BOOST_CHECK(to_string(b) == "00000101");
//
//  b.add("foo");
//  BOOST_CHECK(b.count("foo") == 3);
//  BOOST_CHECK(to_string(b) == "00001111");
//
//  b.add("foo");
//  BOOST_CHECK(b.count("foo") == 4);
//  BOOST_CHECK(to_string(b) == "1010\n00000000");
//
//  b.remove("foo");
//  BOOST_CHECK(b.count("foo") == 3);
//  b.remove("foo");
//  BOOST_CHECK(b.count("foo") == 2);
//  b.remove("foo");
//  BOOST_CHECK(b.count("foo") == 1);
//  BOOST_CHECK(to_string(b) == "00001010");
//}
//BOOST_AUTO_TEST_CASE(a2_bloom_filter)
//{
//  a2<> b({5, 2, 1}, 1);
//
//  b.add("foo");
//  b.add("foo");
//  BOOST_CHECK(b.count("foo") == 1);
//  BOOST_CHECK(to_string(b) == "10100\n00000");
//
//  BOOST_CHECK(b.count("bar") == 1);   // FP
//  b.add("baz"); // Causes core swapping.
//  BOOST_CHECK(to_string(b) == "01000\n11100");
//  b.add("qux"); // Swap again.
//  BOOST_CHECK(to_string(b) == "00001\n01001");
//}
