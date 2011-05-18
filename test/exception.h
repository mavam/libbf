#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>

/// Shortcut
#define THROW(e) BOOST_THROW_EXCEPTION(e)

namespace error {

/// The base exception class for all errors.
/// \note Using virtual inheritance prevents ambiguity in the exception
/// handlers.
/// \see Boost.Exception
struct exception : virtual std::exception, virtual boost::exception { };

/// Configuration error in the config file or on the command line.
struct config : virtual exception { };

/// Mutually excluse options specified.
struct option_conflict : virtual config { };

/// An option dependency was not fulfilled.
struct option_dependency : virtual config { };

/// The user only wants to see help.
struct help_only : virtual config { };

/// Program option.
typedef boost::error_info<struct tag_option, const char*> option;

} // namespace error

#endif
