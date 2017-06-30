#include "configuration.h"

#include <sstream>

std::string config::banner() const {
  std::stringstream ss;
  ss << "    __    ____\n"
        "   / /_  / __/\n"
        "  / __ \\/ /_\n"
        " / /_/ / __/\n"
        "/_.___/_/\n";

  return ss.str();
}

void config::initialize() {
  auto& general = create_block("general options");
  general.add('i', "input", "input file").single();
  general.add('q', "query", "query file").single();
  general.add('h', "help", "display this help");
  general.add('n', "numeric", "interpret input as numeric values");

  auto& bloomfilter = create_block("bloom filter options");
  bloomfilter
    .add('t', "type", "basic|counting|spectral-mi|spectral-rm|bitwise|stable")
    .single();
  bloomfilter.add('f', "fp-rate", "desired false-positive rate").init(0);
  bloomfilter.add('c', "capacity", "max number of expected elements").init(0);
  bloomfilter.add('m', "cells", "number of cells").init(0);
  bloomfilter.add('w', "width", "bits per cells").init(1);
  bloomfilter.add('p', "partition", "enable partitioning");
  bloomfilter.add('e', "evict", "number of cells to evict (stable)").init(0);
  bloomfilter.add('k', "hash-functions", "number of hash functions").init(0);
  bloomfilter.add('d', "double-hashing", "use double-hashing");
  bloomfilter.add('s', "seed", "specify a custom seed").init(0);

  auto& second = create_block("second bloom filter options");
  second.add('M', "cells-2nd", "number of cells").init(0);
  second.add('W', "width-2nd", "bits per cells").init(1);
  second.add('K', "hash-functions-2nd", "number of hash functions").init(0);
  second.add('D', "double-hashing-2nd", "use double-hashing");
  second.add('S', "seed-2nd", "specify a custom seed").init(0);
}
