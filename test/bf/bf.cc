#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "configuration.h"

#include "bf/all.hpp"

using namespace util;
using namespace bf;

trial<nothing> run(config const& cfg) {
  auto numeric = cfg.check("numeric");
  auto k = *cfg.as<size_t>("hash-functions");
  auto cells = *cfg.as<size_t>("cells");
  auto seed = *cfg.as<size_t>("seed");
  auto fpr = *cfg.as<double>("fp-rate");
  auto capacity = *cfg.as<size_t>("capacity");
  auto width = *cfg.as<size_t>("width");
  auto part = cfg.check("partition");
  auto double_hashing = cfg.check("double-hashing");
  auto d = *cfg.as<size_t>("evict");

  auto k2 = *cfg.as<size_t>("hash-functions-2nd");
  auto cells2 = *cfg.as<size_t>("cells-2nd");
  auto seed2 = *cfg.as<size_t>("seed-2nd");
  auto width2 = *cfg.as<size_t>("width-2nd");
  auto double_hashing2 = cfg.check("double-hashing-2nd");

  auto const& type = *cfg.as<std::string>("type");
  std::unique_ptr<bloom_filter> bf;

  if (type == "basic") {
    if (fpr == 0 || capacity == 0) {
      if (cells == 0)
        return error{"need non-zero cells"};
      if (k == 0)
        return error{"need non-zero k"};

      auto h = make_hasher(k, seed, double_hashing);
      bf.reset(new basic_bloom_filter(std::move(h), cells, part));
    } else {
      assert(fpr != 0 && capacity != 0);
      bf.reset(new basic_bloom_filter(fpr, capacity, seed, part));
    }
  } else if (type == "counting") {
    if (cells == 0)
      return error{"need non-zero cells"};
    if (width == 0)
      return error{"need non-zero cell width"};
    if (k == 0)
      return error{"need non-zero k"};

    auto h = make_hasher(k, seed, double_hashing);
    bf.reset(new counting_bloom_filter(std::move(h), cells, width, part));
  } else if (type == "spectral-mi") {
    if (cells == 0)
      return error{"need non-zero cells"};
    if (width == 0)
      return error{"need non-zero cell width"};
    if (k == 0)
      return error{"need non-zero k"};

    auto h = make_hasher(k, seed, double_hashing);
    bf.reset(new spectral_mi_bloom_filter(std::move(h), cells, width, part));
  } else if (type == "spectral-rm") {
    if (cells == 0)
      return error{"need non-zero cells"};
    if (cells2 == 0)
      return error{"need non-zero cells for 2nd bloom filter"};

    if (width == 0)
      return error{"need non-zero cell width"};
    if (width2 == 0)
      return error{"need non-zero cell width for 2nd bloom filter"};

    if (k == 0)
      return error{"need non-zero k"};
    if (k2 == 0)
      return error{"need non-zero k for second bloom filter"};

    auto h1 = make_hasher(k, seed, double_hashing);
    auto h2 = make_hasher(k2, seed2, double_hashing2);
    bf.reset(new spectral_rm_bloom_filter(std::move(h1), cells, width,
                                          std::move(h2), cells2, width2, part));
  } else if (type == "bitwise") {
    if (cells == 0)
      return error{"need non-zero cells"};
    if (k == 0)
      return error{"need non-zero k"};

    bf.reset(new bitwise_bloom_filter(k, cells, seed));
  } else if (type == "a2") {
    if (cells == 0)
      return error{"need non-zero cells"};
    if (capacity == 0)
      return error{"need non-zero capacity"};
    if (k == 0)
      return error{"need non-zero k"};

    bf.reset(new a2_bloom_filter(k, cells, capacity, seed, seed2));
  } else if (type == "stable") {
    if (cells == 0)
      return error{"need non-zero cells"};
    if (k == 0)
      return error{"need non-zero k"};

    auto h = make_hasher(k, seed, double_hashing);
    bf.reset(new stable_bloom_filter(std::move(h), cells, seed, d));
  } else {
    return error{"invalid bloom filter type"};
  }

  std::string line;
  auto input_file = *cfg.as<std::string>("input");
  std::ifstream in{input_file};
  if (!in)
    return error{"cannot read " + input_file};

  in >> std::noskipws;

  while (std::getline(in, line)) {
    if (line.empty())
      continue;

    auto p = line.data();
    while (*p)
      if (*p == ' ' || *p == '\t')
        return error{"whitespace in input not supported"};
      else
        ++p;

    if (numeric)
      bf->add(std::strtod(line.c_str(), nullptr));
    else
      bf->add(line);
  }

  size_t tn = 0, tp = 0, fp = 0, fn = 0;
  size_t ground_truth;
  std::string element;
  auto query_file = *cfg.as<std::string>("query");
  std::ifstream query{query_file};
  if (!query)
    return error{"cannot read " + query_file};

  std::cout << "TN TP FP FN G C E" << std::endl;
  while (query >> ground_truth >> element) // uniq -c
  {
    size_t count;
    if (numeric)
      count = bf->lookup(std::strtod(element.c_str(), nullptr));
    else
      count = bf->lookup(element);

    if (!query)
      return error{"failed to parse element"};

    if (count == 0 && ground_truth == 0)
      ++tn;
    else if (count == ground_truth)
      ++tp;
    else if (count > ground_truth)
      ++fp;
    else
      ++fn;

    std::cout << tn << ' ' << tp << ' ' << fp << ' ' << fn << ' '
              << ground_truth << ' ' << count << ' ';

    if (numeric)
      std::cout << std::strtod(element.c_str(), nullptr);
    else
      std::cout << element;

    std::cout << std::endl;
  }

  return nil;
}

int main(int argc, char* argv[]) {
  auto cfg = config::parse(argc, argv);
  if (!cfg) {
    std::cerr << cfg.failure().msg() << ", try -h or --help" << std::endl;
    return 1;
  }

  if (argc < 2 || cfg->check("help") || cfg->check("advanced")) {
    cfg->usage(std::cerr, cfg->check("advanced"));
    return 0;
  }

  if (!cfg->check("type")) {
    std::cerr << "missing bloom filter type" << std::endl;
    return 1;
  }

  if (!cfg->check("input")) {
    std::cerr << "missing input file" << std::endl;
    return 1;
  }

  if (!cfg->check("query")) {
    std::cerr << "missing query file" << std::endl;
    return 1;
  }

  auto t = run(*cfg);
  if (!t) {
    std::cerr << t.failure().msg() << std::endl;
    return 1;
  }

  return 0;
}
