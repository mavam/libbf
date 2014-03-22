#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <unordered_map>
#include "configuration.h"
#include "bf.h"

using namespace bf;

int main(int argc, char* argv[])
{
  configuration config;
  std::unique_ptr<bloom_filter> bf;
  try
  {
    config.load(argc, argv);
    if (argc < 2 || config.check("help"))
    {
      config.usage(std::cerr);
      return 0;
    }

    auto k = config.as<size_t>("hash-functions");
    auto cells = config.as<size_t>("cells");
    auto seed = config.as<size_t>("seed");
    auto fp = config.as<double>("fp-rate");
    auto capacity = config.as<size_t>("capacity");
    auto width = config.as<size_t>("width");
    auto part = config.as<bool>("partition");
    auto double_hashing = config.as<bool>("double-hashing");
    auto d = config.as<size_t>("evict");

    auto k2 = config.as<size_t>("hash-functions-2nd");
    auto cells2 = config.as<size_t>("cells-2nd");
    auto seed2 = config.as<size_t>("seed-2nd");
    auto width2 = config.as<size_t>("width-2nd");
    auto double_hashing2 = config.as<bool>("double-hashing-2nd");

    auto const& type = config.as<std::string>("type");
    if (type == "basic")
    {
      if (fp == 0 || capacity == 0)
      {
        if (cells == 0)
          throw std::logic_error("need non-zero cells");
        if (k == 0)
          throw std::logic_error("need non-zero k");
        auto h = make_hasher(k, seed, double_hashing);
        bf.reset(new basic_bloom_filter(std::move(h), cells, part));
      }
      else
      {
        assert(fp != 0 && capacity != 0);
        bf.reset(new basic_bloom_filter(fp, capacity, seed, part));
      }
    }
    else if (type == "counting")
    {
      if (cells == 0)
        throw std::logic_error("need non-zero cells");
      if (width == 0)
        throw std::logic_error("need non-zero cell width");
      if (k == 0)
        throw std::logic_error("need non-zero k");
      auto h = make_hasher(k, seed, double_hashing);
      bf.reset( new counting_bloom_filter(std::move(h), cells, width, part));
    }
    else if (type == "spectral-mi")
    {
      if (cells == 0)
        throw std::logic_error("need non-zero cells");
      if (width == 0)
        throw std::logic_error("need non-zero cell width");
      if (k == 0)
        throw std::logic_error("need non-zero k");
      auto h = make_hasher(k, seed, double_hashing);
      bf.reset(new spectral_mi_bloom_filter(std::move(h), cells, width, part));
    }
    else if (type == "spectral-rm")
    {
      if (cells == 0)
        throw std::logic_error("need non-zero cells");
      if (width == 0)
        throw std::logic_error("need non-zero cell width");
      if (k == 0)
        throw std::logic_error("need non-zero k");
      auto h1 = make_hasher(k, seed, double_hashing);
      auto h2 = make_hasher(k2, seed2, double_hashing2);
      bf.reset(new spectral_rm_bloom_filter(std::move(h1), cells, width,
                                            std::move(h2), cells2, width2,
                                            part));
    }
    else if (type == "bitwise")
    {
      if (cells == 0)
        throw std::logic_error("need non-zero cells");
      if (k == 0)
        throw std::logic_error("need non-zero k");
      bf.reset(new bitwise_bloom_filter(k, cells, seed));
    }
    else if (type == "a2")
    {
      if (cells == 0)
        throw std::logic_error("need non-zero cells");
      if (capacity == 0)
        throw std::logic_error("need non-zero capacity");
      if (k == 0)
        throw std::logic_error("need non-zero k");
      bf.reset(new a2_bloom_filter(k, cells, capacity, seed, seed2));
    }
    else if (type == "stable")
    {
      if (cells == 0)
        throw std::logic_error("need non-zero cells");
      if (k == 0)
        throw std::logic_error("need non-zero k");
      auto h = make_hasher(k, seed, double_hashing);
      bf.reset(new stable_bloom_filter(std::move(h), cells, seed, d));
    }
    else
    {
      throw std::logic_error("invalid bloom filter type");
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  std::string line;
  std::ifstream in(config.as<std::string>("input"));
  in >> std::noskipws;
  while (std::getline(in, line))
  {
    if (line.empty())
      continue;
    auto p = line.data();
    while (*p)
      if (*p == ' ' || *p == '\t')
      {
        std::cerr << "whitespace in input not supported" << std::endl;
        return 1;
      }
      else
      {
        ++p;
      }
    bf->add(line);
  }

  size_t tn = 0, tp = 0, fp = 0, fn = 0;
  size_t ground_truth;
  std::string element;
  std::ifstream query(config.as<std::string>("query"));
  std::cout << "TN TP FP FN G C E" << std::endl;
  while (query >> ground_truth >> element)  // uniq -c
  {
    auto count = bf->lookup(line);
    if (count == 0 && ground_truth == 0)
      ++tn;
    else if (count == ground_truth)
      ++tp;
    else if (count > ground_truth)
      ++fp;
    else
      ++fn;
    std::cout
      << tn << ' ' << tp << ' ' << fp << ' ' << fn << ' '
      << ground_truth << ' ' << count << ' ' << element << std::endl;
  }

  return 0;
}
