#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <boost/noncopyable.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

/// The program configuration.
class configuration : boost::noncopyable
{
public:
    /// Constructor.
    /// \param argc The argc parameter from main.
    /// \param argv The argv parameter from main.
    /// \note After instantiating a configuration, init must be called to
    ///     initialize and parse the command line options.
    configuration(int argc, char *argv[]);

    /// Check whether the given option is set.
    /// \param option Name of the option to check.
    /// \return \c true if the given option is set.
    bool check(const char* option) const;

    /// Return the value of the given option.
    /// \param option The name of the option.
    /// \return Option value.
    template <typename T>
    const T& get(const char* option) const
    {
        return config_[option].as<T>();
    }

    /// Prints the program help.
    /// \param out The stream to print the help instructions to.
    /// \param advanced If \c true, print all available program options rather
    ///     than just a small subset.
    void print(std::ostream& out, bool advanced = false) const;

private:
    /// Check that two given options are not specified at the same time.
    /// \param opt1 Option 1.
    /// \param opt2 Option 2.
    void conflicting(const char* opt1, const char* opt2) const;

    /// Check an option dependency.
    /// \param for_what The parameter which depends on another option.
    /// \param required The required parameter.
    void dependency(const char* for_what, const char* required) const;

    po::variables_map config_;              ///< Program configuration.
    po::options_description visible_;       ///< Visible options to the user.
    po::options_description all_;           ///< All existing program options.
};

#endif
