#include "metrics.h"

#include <boost/lexical_cast.hpp>

metrics::metrics()
  : counters_(4, 0)
{
}

void metrics::update(unsigned count, unsigned true_count)
{
    if (count == 0 && true_count == 0)
        ++counters_[metric::tn];
    else if (count == true_count)
        ++counters_[metric::tp];
    else if (count > true_count)
        ++counters_[metric::fp];
    else
        ++counters_[metric::fn];
}

std::string metrics::to_string() const
{
    std::string str;
    auto i = counters_.begin();
    while (i != counters_.end())
    {
        str += boost::lexical_cast<std::string>(*i);
        if (++i != counters_.end())
            str += ' ';
    }

    return str;
}
