#include "configuration.h"

#include <iostream>
#include <boost/thread/thread.hpp>
#include "exception.h"

configuration::configuration(int argc, char *argv[])
  : visible_("")
  , all_("")
{
    po::options_description general("General options");
    general.add_options()
        ("evaluate,E", "compute performance metrics (TP FP FN TN)")
        ("query,q", po::value<std::vector<std::string>>()->composing(),
         "item to query")
        ("query-file,r", po::value<std::string>(),
         "file with one query item per line")
        ("help,h", "display this help")
        ("verbose,v", "be verbose")
        ("advanced,z", "show advanced options")
    ;

    po::options_description bloom_filter("Bloom filter options");
    bloom_filter.add_options()
        ("type,t", po::value<std::string>()->default_value("basic"),
         "bloom filter type, supported values:\n"
         "- basic\n- bitwise\n- spectral-mi\n- spectral-rm\n- a2")
    ;

    typedef std::vector<unsigned> uvec;
    po::options_description core("Core options");
    core.add_options()
        ("hash-functions,k", po::value<uvec>()->composing()->required(),
         "number of hash functions")
        ("size,s", po::value<uvec>()->composing()->required(),
         "number of cells")
        ("width,w", po::value<uvec>()->composing()->required(),
         "bits per cell (fixed-width storage only)")
        ("partitions,p", po::value<uvec>()->composing()->required(),
         "partition the hash into p parts")
        ("double-hashing,d", "use double-hashing")
        ("extended-double-hashing,e", "use extended double-hashing")
    ;

    po::options_description advanced("advanced options");
    advanced.add_options()
        ("output-width,W", po::value<unsigned>()->default_value(10), 
         "number of digits in query display")
//        ("threads,T",
//         po::value<unsigned>()->default_value(
//             boost::thread::hardware_concurrency()),
//         "number of threads")
    ;

    po::positional_options_description p;
    p.add("query", -1);

    all_.add(general).add(bloom_filter).add(core).add(advanced);
    visible_.add(general).add(bloom_filter).add(core);

    po::store(po::command_line_parser(argc, argv).
            options(all_).positional(p).run(), config_);

    if (argc < 2 || check("help") || check("advanced"))
    {
        print(std::cout, check("advanced"));
        THROW(error::help_only());
    }

    po::notify(config_);

    conflicting("double-hashing", "extended-double-hashing");
}

bool configuration::check(const char* option) const
{
    return config_.count(option);
}

void configuration::print(std::ostream& out, bool advanced) const
{
    out << "    __    ____\n"
           "   / /_  / __/\n"
           "  / __ \\/ /_\n"
           " / /_/ / __/\n"
           "/_.___/_/\n"
        << (advanced ? all_ : visible_)
        << std::endl;
}

void configuration::conflicting(const char* opt1, const char* opt2) const
{
    if (check(opt1) && ! config_[opt1].defaulted()
            && check(opt2) && ! config_[opt2].defaulted())
    {
        std::cerr << "option --" << opt1 << " and --" << opt2
            << " are mutually exclusive" << std::endl;
        THROW(error::option_dependency());
    }
}

void configuration::dependency(const char* for_what, const char* required) const
{
    if (check(for_what) && ! config_[for_what].defaulted() &&
            (! check(required) || config_[required].defaulted()))
    {
        std::cerr << "option --" << for_what << " depends on --"
            << required << std::endl;
        THROW(error::option_dependency());
    }
}
