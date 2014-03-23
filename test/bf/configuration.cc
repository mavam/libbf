#include "configuration.h"

#include <sstream>

configuration::configuration()
{
  std::stringstream ss;
  ss << "    __    ____\n"
        "   / /_  / __/\n"
        "  / __ \\/ /_\n"
        " / /_/ / __/\n"
        "/_.___/_/\n";

  banner(ss.str());

  auto& general = create_block("general options");
  general.add('i', "input", "input file");
  general.add('q', "query", "query file");

  auto& bloomfilter = create_block("bloom filter options");
  bloomfilter.add('t', "type",
                  "basic|counting|spectral-mi|spectral-rm|bitwise|stable");
  bloomfilter.add('f', "fp-rate", "desired false-positive rate").init(0);
  bloomfilter.add('c', "capacity", "max number of expected elements").init(0);
  bloomfilter.add('m', "cells", "number of cells").init(0);
  bloomfilter.add('w', "width", "bits per cells").init(1);
  bloomfilter.add('p', "partition", "enable partitioning").init(false);
  bloomfilter.add('e', "evict", "number of cells to evict (stable)").init(0);
  bloomfilter.add('k', "hash-functions", "number of hash functions").init(0);
  bloomfilter.add('d', "double-hashing", "use double-hashing").init(false);
  bloomfilter.add('s', "seed", "specify a custom seed").init(0);

  auto& second = create_block("second bloom filter options");
  second.add('M', "cells-2nd", "number of cells").init(0);
  second.add('W', "width-2nd", "bits per cells").init(1);
  second.add('K', "hash-functions-2nd", "number of hash functions").init(0);
  second.add('D', "double-hashing-2nd", "use double-hashing").init(false);
  second.add('S', "seed-2nd", "specify a custom seed").init(0);
}
