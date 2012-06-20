
                             ___ __    ____  ____
                            / (_) /_  / __ )/ __/
                           / / / __ \/ __  / /_  
                          / / / /_/ / /_/ / __/  
                         /_/_/_.___/_____/_/ 
  

**libbf** is a header-only C++11 library of a of Bloom filters. Its
policy-based design makes it very flexible to customize and allows for creating
new combinations of storage, hashing, and partition properties. libbf currently
supports the follwing types of Bloom filters:

    - Basic (fixed-width)
    - Bitwise
    - Spectral MI
    - Spectral RM
    - A^2
    - Stable


Requirements
=============

    * A C++11 copmiler (GCC >= 4.7 or Clang >= 3.1)
    * CMake (>= 2.8)
    * Boost (>= 1.46)

Installation
============

libbf uses CMake, but provides autotools-like convenience wrappers:

    ./configure --prefix=PREFIX
    make -j 4
    ./build/test/unit
    ./build/test/bf -h

Usage
=====

The following steps show how to use libbf with your own application. All Bloom
filters have (at least) one core. The core is combination of store, hash, and
partition policy. For example, to define a core with fixed-width storage,
double hashing, and partitioning, one would use:

    typedef core<
        fixed_width<uint8_t, std::allocator<uint8_t>
      , double_hashing<default_hasher, 42, 4711>
      , no_partitioning
    > my_core;

Next, we plug the core into the Bloom filter:

      typedef basic<my_core> my_bloom_filter;

Now we can instantiate the Bloom filter. The constructor takes an rvalue
reference to a core:

    my_bloom_filter bf({ 1 << 10, 5, 4 });

That's it, now we can use the Bloom filter as follows:

    bf.add("foo")
    bf.add("foo")
    bf.add('z')
    bf.add(3.14159)
    std::cout << bf.count("foo") << std::endl;  // returns 2

libbf also ships with a test suite and a small Bloom filter test tool `bf`.
This command-line program allows you to select one of the Bloom filter
implementations, feed it with data via STDIN, and finally execute queries after
the set has been populated.

License
========

libbf ships with a BSD-style license (see `COPYING` for details).
