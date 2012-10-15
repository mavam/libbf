#include <fstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <boost/cstdlib.hpp>
#include "bloom_filter/a2.h"
#include "bloom_filter/basic.h"
#include "bloom_filter/bitwise.h"
#include "bloom_filter/spectral.h"
#include "bloom_filter/stable.h"
#include "configuration.h"
#include "metrics.h"
#include "exception.h"

typedef bf::basic<>::core_type::store_type fixed_width;
typedef bf::default_hashing<> default_hash;
typedef bf::double_hashing<> double_hash;
typedef bf::core<fixed_width, default_hash, bf::partitioning> part;
typedef bf::core<fixed_width, double_hash> dbl;
typedef bf::core<fixed_width, double_hash, bf::partitioning> dbl_part;

template <typename BloomFilter>
void run(BloomFilter&& bf, const configuration& config)
{
    std::cin >> std::noskipws;
    std::string line;

    bool verbose = config.check("verbose");
    bool evaluate = config.check("evaluate");
    metrics stream;
    metrics query;
    unsigned count;
    unsigned true_count;
    std::unordered_map<std::string, unsigned> history;

    while (std::getline(std::cin, line))
        if (! line.empty())
        {
            bf.add(line);
            if (evaluate)
            {
                count = bf.count(line);
                true_count = ++history[line];
                stream.update(count, true_count);

                if (verbose && count != true_count)
                    std::cerr << line << ' ' << true_count << ' ' << count
                        << std::endl;
            }
        }

    auto width = config.get<unsigned>("output-width");

    if (config.check("query"))
        for (const auto& q : config.get<std::vector<std::string>>("query"))
            if (! q.empty())
            {
                std::cout << std::setw(width) << bf.count(q) << " " 
                    << q << std::endl;

                if (evaluate)
                {
                    count = bf.count(line);
                    true_count = history[line];
                    query.update(count, true_count);
                }
            }

    if (config.check("query-file"))
    {
        std::ifstream ifs(config.get<std::string>("query-file").c_str());
        while (std::getline(ifs, line))
            if (! line.empty())
            {
                std::cout << std::setw(width) << bf.count(line) << " " 
                    << line << std::endl;

                if (evaluate)
                {
                    count = bf.count(line);
                    true_count = history[line];
                    query.update(count, true_count);
                }
            }
    }

    if (evaluate)
    {
        const auto& s = config.get<std::vector<unsigned>>("size");
        const auto& k = config.get<std::vector<unsigned>>("hash-functions");
        const auto& w = config.get<std::vector<unsigned>>("width");
        const auto& p = config.get<std::vector<unsigned>>("partitions");

        std::cerr << config.get<std::string>("type") << ' ';
        std::cerr << "core1 ";
        std::cerr << s.front() << ' ';
        std::cerr << k.front() << ' ';
        std::cerr << w.front() << ' ';
        std::cerr << p.front() << ' ';
        std::cerr << "core2 ";
        std::cerr << s.back() << ' ';
        std::cerr << k.back() << ' ';
        std::cerr << w.back() << ' ';
        std::cerr << p.back() << ' ';

        std::cerr << "stream " << stream.to_string();
        if (config.check("query") || config.check("query-file"))
            std::cerr << " query " << query.to_string();

        std::cerr << std::endl;
    }
}

template <typename BF1, typename BF2, typename BF3, typename BF4>
void dispatch1(unsigned s, unsigned k, unsigned w, unsigned p,
        const configuration& config)
{
    bool double_hash = config.check("double-hashing");
    bool extended_double_hash = config.check("extended-double-hashing");
    bool default_hash = ! (double_hash || extended_double_hash);

    if (default_hash)
    {
        if (p == 1)
            run(BF1({ s, k, w }), config);
        else 
            run(BF2({ s, k, w, p }), config);
    }
    else if (double_hash)
    {
        if (p == 1)
            run(BF3({ s, k, w }), config);
        else 
            run(BF4({ s, k, w, p }), config);
    }
}

template <typename BF1, typename BF2, typename BF3, typename BF4>
void dispatch2(unsigned s1, unsigned k1, unsigned w1, unsigned p1,
        unsigned s2, unsigned k2, unsigned w2, unsigned p2,
        const configuration& config)
{
    bool double_hash = config.check("double-hashing");
    bool extended_double_hash = config.check("extended-double-hashing");
    bool default_hash = ! (double_hash || extended_double_hash);

    if (default_hash)
    {
        if (p1 == 1)
            run(BF1({ s2, k2, w2 }, { s2, k2, w2 }), config);
        else 
            run(BF2({ s1, k1, w1, p1 }, { s2, k2, w2, p2 }), config);
    }
    else if (double_hash)
    {
        if (p1 == 1)
            run(BF3({ s2, k2, w2 }, { s2, k2, w2 }), config);
        else 
            run(BF4({ s1, k1, w1, p1 }, { s2, k2, w2, p2 }), config);
    }
}

int main(int argc, char* argv[])
{
    try
    {
        configuration config(argc, argv);

        const auto& s = config.get<std::vector<unsigned>>("size");
        const auto& k = config.get<std::vector<unsigned>>("hash-functions");
        const auto& w = config.get<std::vector<unsigned>>("width");
        const auto& p = config.get<std::vector<unsigned>>("partitions");

        const auto& type = config.get<std::string>("type");
        if (type == "a2")
            dispatch1<
                bf::a2<>,
                bf::a2<part>,
                bf::a2<dbl>,
                bf::a2<dbl_part>
            >(s.back(), k.back(), w.back(), p.back(), config);
        if (type == "basic")
            dispatch1<
                bf::basic<>,
                bf::basic<part>,
                bf::basic<dbl>,
                bf::basic<dbl_part>
            >(s.back(), k.back(), w.back(), p.back(), config);
        else if (type == "bitwise")
            dispatch1<
                bf::bitwise<>,
                bf::bitwise<part>,
                bf::bitwise<dbl>,
                bf::bitwise<dbl_part>
            >(s.back(), k.back(), w.back(), p.back(), config);
        else if (type == "spectral-mi")
        {
            dispatch1<
                bf::spectral_mi<>,
                bf::spectral_mi<part>,
                bf::spectral_mi<dbl>,
                bf::spectral_mi<dbl_part>
            >(s.back(), k.back(), w.back(), p.back(), config);
        }
        else if (type == "spectral-rm")
        {
            dispatch2<
                bf::spectral_rm<>,
                bf::spectral_rm<part>,
                bf::spectral_rm<dbl>,
                bf::spectral_rm<dbl_part>
            >(s.front(), k.front(), w.front(), p.front(),
              s.back(), k.back(), w.back(), p.back(), 
              config);
        }
    }
    catch (const boost::program_options::required_option& e)
    {
        std::cerr << e.what() << std::endl;
        return boost::exit_exception_failure;
    }
    catch (const boost::program_options::unknown_option& e)
    {
        std::cerr << e.what() << ", try -h or --help" << std::endl;
        return boost::exit_exception_failure;
    }
    catch (const error::help_only& e)
    {
        return boost::exit_success;
    }
    catch (const error::config& e)
    {
        if (const char* const* info =
                boost::get_error_info<error::option>(e))
            std::cerr << "invalid use of option --" << *info << std::endl;

        return boost::exit_exception_failure;
    }
    catch (const boost::exception& e)
    {
        std::cerr << boost::diagnostic_information(e);
        return boost::exit_exception_failure;
    }

    return boost::exit_success;
}
