#ifndef METRICS_H
#define METRICS_H

#include <string>
#include <vector>

/// The performance metrics.
class metrics
{
public:
    /// The different metric types.
    enum metric
    {
        tp = 0,     ///< True positive.
        fp = 1,     ///< False positive.
        fn = 2,     ///< False negative.
        tn = 3      ///< True negative.
    };

public:
    /// Create a metrics object. 
    metrics();

    /// Update performance metrics.
    /// \param count The count from the Bloom filter.
    /// \param true_count The true count from the data.
    void update(unsigned count, unsigned true_count);

    /// Get a string representation of the metrics. The result is a 4-tuple of
    /// the form \f$(TP, FP, FN, TN)\f$.
    /// \return A space-delimited representing of the counters. 
    std::string to_string() const;

private:
    std::vector<unsigned> counters_;
};

#endif
