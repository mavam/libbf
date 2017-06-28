#include "test.hpp"

#include "bf/all.hpp"

using namespace bf;

TEST(counter_vector_incrementing_width2) {
  counter_vector v(3, 2);
  // Increment 1/3
  CHECK(v.increment(0));
  CHECK_EQUAL(to_string(v), "100000");
  CHECK_EQUAL(v.count(0), 1u);
  // Increment 2/3
  CHECK(v.increment(0));
  CHECK_EQUAL(to_string(v), "010000");
  CHECK_EQUAL(v.count(0), 2u);
  // Increment 3/3
  CHECK(v.increment(0));
  CHECK_EQUAL(to_string(v), "110000");
  CHECK_EQUAL(v.count(0), 3u);
  // Already reached maximum counter value 3 with 2 bits.
  CHECK(!v.increment(0));
  CHECK_EQUAL(to_string(v), "110000");
  CHECK_EQUAL(v.count(0), 3u);
  // Increment adjacent value.
  CHECK(v.increment(1));
  CHECK_EQUAL(to_string(v), "111000");
  CHECK_EQUAL(v.count(1), 1u);
  // And another random one.
  CHECK(v.increment(2));
  CHECK_EQUAL(to_string(v), "111010");
  CHECK_EQUAL(v.count(2), 1u);
}

TEST(counter_vector_incrementing_width3) {
  counter_vector v(3, 3);
  // Increment 1/7
  CHECK(v.increment(1));
  CHECK_EQUAL(to_string(v), "000100000");
  CHECK_EQUAL(v.count(1), 1u);
  // Increment 2/7
  CHECK(v.increment(1));
  CHECK_EQUAL(to_string(v), "000010000");
  CHECK_EQUAL(v.count(1), 2u);
  // Increment 3/7
  CHECK(v.increment(1));
  CHECK_EQUAL(to_string(v), "000110000");
  CHECK_EQUAL(v.count(1), 3u);
  // Increment 4/7
  CHECK(v.increment(1));
  CHECK_EQUAL(to_string(v), "000001000");
  CHECK_EQUAL(v.count(1), 4u);
  // Increment += 3 to 7/7
  CHECK(v.increment(1, 3));
  CHECK_EQUAL(to_string(v), "000111000");
  CHECK_EQUAL(v.count(1), 7u);
  // Reset
  v.clear();
  CHECK_EQUAL(to_string(v), "000000000");
  CHECK(v.increment(1, 6));
  CHECK_EQUAL(to_string(v), "000011000");
  CHECK_EQUAL(v.count(1), 6u);
}

TEST(counter_vector_decrementing_width3) {
  counter_vector v(3, 3);
  // Increment to max value of 7.
  CHECK(v.increment(1, 7));
  CHECK_EQUAL(v.count(1), 7u);
  CHECK_EQUAL(to_string(v), "000111000");
  // Decrement by 1.
  CHECK(v.decrement(1));
  CHECK_EQUAL(to_string(v), "000011000");
  CHECK_EQUAL(v.count(1), 6u);
  // Decrement by 1, again.
  CHECK(v.decrement(1));
  CHECK_EQUAL(to_string(v), "000101000");
  CHECK_EQUAL(v.count(1), 5u);
  // Decrement by 1, again.
  CHECK(v.decrement(1));
  CHECK_EQUAL(to_string(v), "000001000");
  CHECK_EQUAL(v.count(1), 4u);
  // Increment by 1, then decrement by 3.
  CHECK(v.increment(1));
  CHECK(v.decrement(1, 3));
  CHECK_EQUAL(to_string(v), "000010000");
  CHECK_EQUAL(v.count(1), 2u);
}

TEST(counter_vector_adding) {
  counter_vector v(2, 3);
  // Increment to 3.
  CHECK(v.increment(0, 3));
  CHECK_EQUAL(to_string(v), "110000");
  CHECK_EQUAL(v.count(0), 3u);
  // Increment to 4.
  CHECK(v.increment(0, 1) == 1);
  CHECK_EQUAL(to_string(v), "001000");
  CHECK_EQUAL(v.count(0), 4u);
  // Increment to 5.
  CHECK(v.increment(0, 1));
  CHECK_EQUAL(v.count(0), 5u);
  CHECK_EQUAL(to_string(v), "101000");
  // Increment to 7.
  CHECK(!v.increment(0, 3));
  CHECK_EQUAL(to_string(v), "111000");
  CHECK_EQUAL(v.count(0), 7u);
  // Go to 2nd cell, repeat.
  CHECK(v.increment(1, 4));
  CHECK_EQUAL(to_string(v), "111001");
  CHECK_EQUAL(v.count(1), 4u);
  // Fill it up.
  CHECK(v.increment(1, 3));
  CHECK_EQUAL(to_string(v), "111111");
  // Cannot increment at max cell value.
  CHECK(!v.increment(1));
  CHECK(!v.increment(1, 42));
}

TEST(counter_vector_adding_big) {
  counter_vector v(3, 32);
  auto max = std::numeric_limits<uint32_t>::max();
  REQUIRE_EQUAL(v.max(), max);
  std::string value;
  size_t step = 1 << 15;
  size_t last = 0;
  for (size_t i = 0; i < max && last <= i; i += step) {
    last = i;
    value = std::to_string(i);
    REQUIRE(v.count(0) == i);
    v.increment(0, step);
  }
}

TEST(counter_vector_mergin) {
  counter_vector a(5, 2);
  counter_vector b(5, 2);
  a.increment(0, 1);
  a.increment(1, 1);
  a.increment(2, 2);
  b.increment(1, 1);
  b.increment(2, 1);
  b.increment(3, 3);
  CHECK_EQUAL(to_string(a | b), "1001111100");
}

TEST(bloom_filter_basic) {
  basic_bloom_filter bf(0.8, 10);
  bf.add("foo");
  bf.add("bar");
  bf.add("baz");
  bf.add('c');
  bf.add(4.2);
  bf.add(4711ULL);
  // True-positives
  CHECK_EQUAL(bf.lookup("foo"), 1u);
  CHECK_EQUAL(bf.lookup("bar"), 1u);
  CHECK_EQUAL(bf.lookup("baz"), 1u);
  CHECK_EQUAL(bf.lookup(4.2), 1u);
  CHECK_EQUAL(bf.lookup('c'), 1u);
  CHECK_EQUAL(bf.lookup(4711ULL), 1u);
  // True-negatives
  CHECK_EQUAL(bf.lookup("qux"), 0u);
  CHECK_EQUAL(bf.lookup("graunt"), 0u);
  CHECK_EQUAL(bf.lookup(3.1415), 0u);
  // False-positives
  CHECK_EQUAL(bf.lookup("corge"), 1u);
  CHECK_EQUAL(bf.lookup('a'), 1u);

  // another filter
  basic_bloom_filter obf(0.8, 10);
  obf.swap(bf);

  CHECK_EQUAL(obf.lookup("foo"), 1u);

  // Make bf using another filter's storage
  hasher h = obf.hasher_function();
  bitvector b = obf.storage();
  basic_bloom_filter obfc(h, b);
  CHECK_EQUAL(obfc.storage(), b);
  CHECK_EQUAL(obfc.lookup("foo"), 1u);
}

TEST(bloom_filter_counting) {
  counting_bloom_filter bf(make_hasher(3), 10, 2);
  for (size_t i = 0; i < 3; ++i) {
    bf.add("qux");
    bf.add("corge");
    bf.add("grault");
    bf.add(3.14159265);
  }
  CHECK_EQUAL(bf.lookup("foo"), 0u);
  CHECK_EQUAL(bf.lookup("qux"), 3u);
  CHECK_EQUAL(bf.lookup("corge"), 3u);
  CHECK_EQUAL(bf.lookup("grault"), 3u);
  CHECK_EQUAL(bf.lookup(3.14159265), 3u);
  for (size_t i = 0; i < 3; ++i)
    bf.remove("grault");
  CHECK_EQUAL(bf.lookup("corge"), 0u);
}

TEST(bloom_filter_spectral_mi) {
  spectral_mi_bloom_filter bf(make_hasher(3), 8, 2);
  bf.add("oh");
  bf.add("oh");
  bf.add("my");
  bf.add("god");
  bf.add("becky");
  bf.add("look");
  CHECK_EQUAL(bf.lookup("oh"), 2u);
  CHECK_EQUAL(bf.lookup("my"), 1u);
  CHECK_EQUAL(bf.lookup("god"), 1u);
  CHECK_EQUAL(bf.lookup("becky"), 1u);
  CHECK_EQUAL(bf.lookup("look"), 2u); // FP, same cells as "god".
}

TEST(bloom_filter_spectral_rm) {
  auto h1 = make_hasher(3, 0);
  auto h2 = make_hasher(3, 1);
  spectral_rm_bloom_filter bf(std::move(h1), 5, 2, std::move(h2), 4, 2);
  bf.add("foo");
  CHECK_EQUAL(bf.lookup("foo"), 1u);
  // TODO: port old unit tests and double-check the implementation.

  //// For "bar", all hash functions return the same position, the we have
  //// necessarily a recurring minimum (RM). Thus we do not look in the second
  //// core and return 2, although the correct count would be 1.
  // b.add("bar"); // 2 0 1 0 0 and 0 0
  // CHECK(b.count("bar") == 2);
  // CHECK(to_string(b) == "010000100000000\n0000");
  //// For "foo", we encounter a unique minimum in the first core, but since
  //// all positions for "foo" are zero in the second core, we return the
  //// mimimum of the first, which is 1.
  // CHECK(b.count("foo") == 1);
  //// After increasing the counters for "foo", we find that it (still) has a
  //// unique minimum in in the first core. Hence we add its minimum to the
  //// second core.
  // b.add("foo"); // 3 0 2 0 0 and 2 2
  // CHECK(b.count("foo") == 2);
  // CHECK(to_string(b) == "110000010000000\n0101");
  //// The "blue fish" causes some trouble: because its insertion yields a
  //// unique minimum, we go into the second bitvector. There, we find that it
  //// hashes to the same positions as foo, wich has a counter of 2. Because it
  //// appears to exist there, we have to increment its counters. This falsely
  //// bumps up the counter of "blue fish" to 3.
  // b.add("blue fish"); // 3 0 3 0 1 and 3 3
  // CHECK(b.count("blue fish") == 3);
  // CHECK(to_string(b) == "110000110000100\n1111");
  //// Since the "blue fish" has (still) a unique minimum after removing it one
  //// time, we look in the second core and find it to be present there.
  //// Hence we decrement the counters in the second core.
  // b.remove("blue fish"); // 3 0 2 0 0 and 2 2
  // CHECK(b.count("blue fish") == 2);
  // CHECK(to_string(b) == "110000010000000\n0101");
  // b.remove("blue fish");
  // CHECK(b.count("blue fish") == 1); // 3 0 1 0 0 and 1 1
  //// Let's look at "foo". This fellow has now a unique minimum. Since it has
  //// a unique minimum after the removal, we also decrement the counter in the
  //// second core.
  // b.remove("foo"); // 2 0 0 0 0 and 0 0
  // CHECK(b.count("foo") == 0);
  // CHECK(to_string(b) == "010000000000000\n0000");
  //// Alas, we violated Claim 1 in Section 2.2 in the paper! The spectral
  //// Bloom filter returns a count of 0 for "foo", although it should be 1.
  //// Thus, the frequency estimate is no longer a lower bound. This occurs
  //// presumably due to the fact that we remove "blue fish" twice although we
  //// added it only once.
}

TEST(bloom_filter_bitwise) {
  bitwise_bloom_filter bf(3, 8);
  CHECK_EQUAL(bf.lookup("foo"), 0u);
  bf.add("foo");
  CHECK_EQUAL(bf.lookup("foo"), 1u);
  bf.add("foo");
  CHECK_EQUAL(bf.lookup("foo"), 2u);
  bf.add("foo");
  CHECK_EQUAL(bf.lookup("foo"), 3u);
  // Other elements.
  CHECK_EQUAL(bf.lookup("baz"), 0u);
  bf.add("baz");
  CHECK_EQUAL(bf.lookup("baz"), 1u);
  CHECK_EQUAL(bf.lookup("foo"), 3u);
  bf.add("baz");
  CHECK_EQUAL(bf.lookup("baz"), 2u);
  CHECK_EQUAL(bf.lookup("foo"), 3u);
}

TEST(bloom_filter_stable) {
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
  CHECK_EQUAL(bf.lookup("one fish"), 0u);
  CHECK_EQUAL(bf.lookup("two fish"), 2u);
  CHECK_EQUAL(bf.lookup("red fish"), 3u);
  CHECK_EQUAL(bf.lookup("blue fish"), 3u);
}

TEST(bloom_filter_a2) {
  a2_bloom_filter bf(3, 32, 3);
  bf.add("foo");
  bf.add("foo"); // Duplicate inserts have no effect.
  bf.add("bar");
  bf.add("baz");
  // Reaches capacity and causes swapping.
  bf.add("qux");
  CHECK_EQUAL(bf.lookup("foo"), 1u);
  CHECK_EQUAL(bf.lookup("bar"), 1u);
  CHECK_EQUAL(bf.lookup("baz"), 1u);
  CHECK_EQUAL(bf.lookup("qux"), 1u);
}
