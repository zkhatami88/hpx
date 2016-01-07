//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file throw.hpp

#if !defined(HPX_EXCEPTION_THROW_HPP)
#define HPX_EXCEPTION_THROW_HPP

#include <hpx/config.hpp>
#include <hpx/error.hpp>
#include <hpx/exception_fwd.hpp>
#include <hpx/exception/exception.hpp>
#include <hpx/exception/get_error.hpp>
#include <hpx/exception/get_error_what.hpp>
#include <hpx/exception/get_error_file_name.hpp>
#include <hpx/exception/get_error_line_number.hpp>
#include <hpx/exception/get_exception.hpp>
#include <hpx/exception/throwmode.hpp>
#include <hpx/error.hpp>
#include <hpx/util/filesystem_compatibility.hpp>

#include <boost/current_function.hpp>
#include <boost/filesystem/path.hpp>

#include <string>

namespace hpx { namespace detail {

    // main function for throwing exceptions
    template <typename Exception>
    HPX_ATTRIBUTE_NORETURN HPX_EXPORT
    void throw_exception(Exception const& e,
        std::string const& func, std::string const& file, long line);
}}

/// \cond NOINTERNAL
///////////////////////////////////////////////////////////////////////////////
// helper macro allowing to prepend file name and line number to a generated
// exception
#define HPX_THROW_EXCEPTION_(except, errcode, func, msg, mode, file, line)    \
    {                                                                         \
        hpx::detail::throw_exception(                                         \
            except(static_cast<hpx::error>(errcode), msg, mode),              \
            func, file, line);                                                \
    }                                                                         \
    /**/

#define HPX_THROW_STD_EXCEPTION(except, func)                                 \
    {                                                                         \
        boost::filesystem::path p__(hpx::util::create_path(__FILE__));        \
        hpx::detail::throw_exception(except, func, p__.string(), __LINE__);   \
    }                                                                         \
    /**/

#define HPX_RETHROW_EXCEPTION(e, f)                                           \
    HPX_THROW_EXCEPTION_(hpx::exception, hpx::get_error(e), f,                \
        hpx::get_error_what(e), hpx::rethrow, hpx::get_error_file_name(e),    \
        hpx::get_error_line_number(e))                                        \
    /**/

#define HPX_RETHROWS_IF(ec, e, f)                                             \
    {                                                                         \
        if (&ec == &hpx::throws) {                                            \
            HPX_RETHROW_EXCEPTION(e, f);                                      \
        } else {                                                              \
            ec = make_error_code(hpx::get_error(e), hpx::get_error_what(e),   \
                f, hpx::get_error_file_name(e).c_str(), hpx::get_error_line_number(e), \
                hpx::get_rethrowmode(ec));                                    \
        }                                                                     \
    }                                                                         \
    /**/

///////////////////////////////////////////////////////////////////////////////
#define HPX_GET_EXCEPTION_(except, errcode, func, msg, mode, file, line)      \
    hpx::detail::get_exception(                                               \
        except(static_cast<hpx::error>(errcode), msg, mode),                  \
        func, file, line)                                                    \
/**/

#define HPX_GET_EXCEPTION(errcode, f, msg)                                    \
    HPX_GET_EXCEPTION_(hpx::exception, errcode, f, msg, hpx::plain,           \
        boost::filesystem::path(hpx::util::create_path(__FILE__)).string(),   \
        __LINE__)                                                             \
/**/

///////////////////////////////////////////////////////////////////////////////
#define HPX_THROW_IN_CURRENT_FUNC(errcode, msg)                               \
    HPX_THROW_EXCEPTION(errcode, BOOST_CURRENT_FUNCTION, msg)                 \
    /**/

#define HPX_RETHROW_IN_CURRENT_FUNC(errcode, msg)                             \
    HPX_RETHROW_EXCEPTION(errcode, BOOST_CURRENT_FUNCTION, msg)               \
    /**/

///////////////////////////////////////////////////////////////////////////////
#define HPX_THROWS_IN_CURRENT_FUNC_IF(ec, errcode, msg)                       \
    {                                                                         \
        if (&ec == &hpx::throws) {                                            \
            HPX_THROW_EXCEPTION(errcode, BOOST_CURRENT_FUNCTION, msg);        \
        } else {                                                              \
            ec = make_error_code(static_cast<hpx::error>(errcode), msg,       \
                BOOST_CURRENT_FUNCTION, __FILE__, __LINE__,                   \
                hpx::get_throwmode(ec));                                      \
        }                                                                     \
    }                                                                         \
    /**/

#define HPX_RETHROWS_IN_CURRENT_FUNC_IF(ec, errcode, msg)                     \
    {                                                                         \
        if (&ec == &hpx::throws) {                                            \
            HPX_RETHROW_EXCEPTION(errcode, BOOST_CURRENT_FUNCTION, msg);      \
        } else {                                                              \
            ec = make_error_code(static_cast<hpx::error>(errcode), msg,       \
                BOOST_CURRENT_FUNCTION, __FILE__, __LINE__,                   \
                hpx::get_rethrowmode(ec));                                    \
        }                                                                     \
    }                                                                         \
    /**/
/// \endcond

///////////////////////////////////////////////////////////////////////////////
/// \def HPX_THROW_EXCEPTION(errcode, f, msg)
/// \brief Throw a hpx::exception initialized from the given parameters
///
/// The macro \a HPX_THROW_EXCEPTION can be used to throw a hpx::exception.
/// The purpose of this macro is to prepend the source file name and line number
/// of the position where the exception is thrown to the error message.
/// Moreover, this associates additional diagnostic information with the
/// exception, such as file name and line number, locality id and thread id,
/// and stack backtrace from the point where the exception was thrown.
///
/// The parameter \p errcode holds the hpx::error code the new exception should
/// encapsulate. The parameter \p f is expected to hold the name of the
/// function exception is thrown from and the parameter \p msg holds the error
/// message the new exception should encapsulate.
///
/// \par Example:
///
/// \code
///      void raise_exception()
///      {
///          // Throw a hpx::exception initialized from the given parameters.
///          // Additionally associate with this exception some detailed
///          // diagnostic information about the throw-site.
///          HPX_THROW_EXCEPTION(hpx::no_success, "raise_exception", "simulated error");
///      }
/// \endcode
///
#define HPX_THROW_EXCEPTION(errcode, f, msg)                                  \
    {                                                                         \
        boost::filesystem::path p__(hpx::util::create_path(__FILE__));        \
        HPX_THROW_EXCEPTION_(hpx::exception, errcode, f, msg, hpx::plain,     \
            p__.string(), __LINE__)                                           \
    }                                                                         \
    /**/

/// \def HPX_THROWS_IF(ec, errcode, f, msg)
/// \brief Either throw a hpx::exception or initialize \a hpx::error_code from
///        the given parameters
///
/// The macro \a HPX_THROWS_IF can be used to either throw a \a hpx::exception
/// or to initialize a \a hpx::error_code from the given parameters. If
/// &ec == &hpx::throws, the semantics of this macro are equivalent to
/// \a HPX_THROW_EXCEPTION. If &ec != &hpx::throws, the \a hpx::error_code
/// instance \p ec is initialized instead.
///
/// The parameter \p errcode holds the hpx::error code from which the new
/// exception should be initialized. The parameter \p f is expected to hold the
/// name of the function exception is thrown from and the parameter \p msg
/// holds the error message the new exception should encapsulate.
///
#define HPX_THROWS_IF(ec, errcode, f, msg)                                    \
    {                                                                         \
        if (&ec == &hpx::throws) {                                            \
            HPX_THROW_EXCEPTION(errcode, f, msg);                             \
        } else {                                                              \
            ec = make_error_code(static_cast<hpx::error>(errcode), msg, f,    \
                __FILE__, __LINE__,                                           \
                hpx::get_throwmode(ec));                                      \
        }                                                                     \
    }                                                                         \
    /**/

#endif

