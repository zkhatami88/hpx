//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file error_code.hpp

#if !defined(HPX_EXCEPTION_ERROR_CODE_HPP)
#define HPX_EXCEPTION_ERROR_CODE_HPP

#include <hpx/config.hpp>
#include <hpx/exception_fwd.hpp>
#include <hpx/exception/throwmode.hpp>
#include <hpx/exception/detail/access_exception.hpp>
#include <hpx/error.hpp>

#include <boost/exception_ptr.hpp>
#include <boost/system/error_code.hpp>

#include <string>

///////////////////////////////////////////////////////////////////////////////
namespace hpx
{
    ///////////////////////////////////////////////////////////////////////////
    /// \brief A hpx::error_code represents an arbitrary error condition.
    ///
    /// The class hpx::error_code describes an object used to hold error code
    /// values, such as those originating from the operating system or other
    /// low-level application program interfaces.
    ///
    /// \note Class hpx::error_code is an adjunct to error reporting by
    /// exception
    ///
    class error_code : public boost::system::error_code //-V690
    {
    public:
        /// Construct an object of type error_code.
        ///
        /// \param mode   The parameter \p mode specifies whether the constructed
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        ///
        /// \throws nothing
        explicit error_code(throwmode mode = plain);

        /// Construct an object of type error_code.
        ///
        /// \param e      The parameter \p e holds the hpx::error code the new
        ///               exception should encapsulate.
        /// \param mode   The parameter \p mode specifies whether the constructed
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        ///
        /// \throws nothing
        explicit error_code(error e, throwmode mode = plain);

        /// Construct an object of type error_code.
        ///
        /// \param e      The parameter \p e holds the hpx::error code the new
        ///               exception should encapsulate.
        /// \param func   The name of the function where the error was raised.
        /// \param file   The file name of the code where the error was raised.
        /// \param line   The line number of the code line where the error was
        ///               raised.
        /// \param mode   The parameter \p mode specifies whether the constructed
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        ///
        /// \throws nothing
        error_code(error e, char const* func, char const* file,
            long line, throwmode mode = plain);

        /// Construct an object of type error_code.
        ///
        /// \param e      The parameter \p e holds the hpx::error code the new
        ///               exception should encapsulate.
        /// \param msg    The parameter \p msg holds the error message the new
        ///               exception should encapsulate.
        /// \param mode   The parameter \p mode specifies whether the constructed
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        ///
        /// \throws std#bad_alloc (if allocation of a copy of
        ///         the passed string fails).
        error_code(error e, char const* msg, throwmode mode = plain);

        /// Construct an object of type error_code.
        ///
        /// \param e      The parameter \p e holds the hpx::error code the new
        ///               exception should encapsulate.
        /// \param msg    The parameter \p msg holds the error message the new
        ///               exception should encapsulate.
        /// \param func   The name of the function where the error was raised.
        /// \param file   The file name of the code where the error was raised.
        /// \param line   The line number of the code line where the error was
        ///               raised.
        /// \param mode   The parameter \p mode specifies whether the constructed
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        ///
        /// \throws std#bad_alloc (if allocation of a copy of
        ///         the passed string fails).
        error_code(error e, char const* msg, char const* func,
                char const* file, long line, throwmode mode = plain);

        /// Construct an object of type error_code.
        ///
        /// \param e      The parameter \p e holds the hpx::error code the new
        ///               exception should encapsulate.
        /// \param msg    The parameter \p msg holds the error message the new
        ///               exception should encapsulate.
        /// \param mode   The parameter \p mode specifies whether the constructed
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        ///
        /// \throws std#bad_alloc (if allocation of a copy of
        ///         the passed string fails).
        error_code(error e, std::string const& msg, throwmode mode = plain);

        /// Construct an object of type error_code.
        ///
        /// \param e      The parameter \p e holds the hpx::error code the new
        ///               exception should encapsulate.
        /// \param msg    The parameter \p msg holds the error message the new
        ///               exception should encapsulate.
        /// \param func   The name of the function where the error was raised.
        /// \param file   The file name of the code where the error was raised.
        /// \param line   The line number of the code line where the error was
        ///               raised.
        /// \param mode   The parameter \p mode specifies whether the constructed
        ///               hpx::error_code belongs to the error category
        ///               \a hpx_category (if mode is \a plain, this is the
        ///               default) or to the category \a hpx_category_rethrow
        ///               (if mode is \a rethrow).
        ///
        /// \throws std#bad_alloc (if allocation of a copy of
        ///         the passed string fails).
        error_code(error e, std::string const& msg, char const* func,
                char const* file, long line, throwmode mode = plain);

        /// Return a reference to the error message stored in the hpx::error_code.
        ///
        /// \throws nothing
        std::string get_message() const;

        /// \brief Clear this error_code object.
        /// The postconditions of invoking this method are
        /// * value() == hpx::success and category() == hpx::get_hpx_category()
        void clear();

        /// Assignment operator for error_code
        ///
        /// \note This function maintains the error category of the left hand
        ///       side if the right hand side is a success code.
        error_code& operator=(error_code const& rhs);

    private:
        friend boost::exception_ptr detail::access_exception(error_code const&);
        friend class exception;
        friend error_code make_error_code(boost::exception_ptr const&);

        error_code(int err, hpx::exception const& e);
        explicit error_code(boost::exception_ptr const& e);

        boost::exception_ptr exception_;
    };

    /// @{
    /// \brief Returns a new error_code constructed from the given parameters.
    inline error_code
    make_error_code(error e, throwmode mode = plain)
    {
        return error_code(e, mode);
    }
    inline error_code
    make_error_code(error e, char const* func, char const* file, long line,
        throwmode mode = plain)
    {
        return error_code(e, func, file, line, mode);
    }

    /// \brief Returns error_code(e, msg, mode).
    inline error_code
    make_error_code(error e, char const* msg, throwmode mode = plain)
    {
        return error_code(e, msg, mode);
    }
    inline error_code
    make_error_code(error e, char const* msg, char const* func,
        char const* file, long line, throwmode mode = plain)
    {
        return error_code(e, msg, func, file, line, mode);
    }

    /// \brief Returns error_code(e, msg, mode).
    inline error_code
    make_error_code(error e, std::string const& msg, throwmode mode = plain)
    {
        return error_code(e, msg, mode);
    }
    inline error_code
    make_error_code(error e, std::string const& msg, char const* func,
        char const* file, long line, throwmode mode = plain)
    {
        return error_code(e, msg, func, file, line, mode);
    }
    inline error_code
    make_error_code(boost::exception_ptr const& e)
    {
        return error_code(e);
    }
    ///@}

    /// \brief Returns error_code(hpx::success, "success", mode).
    inline error_code
    make_success_code(throwmode mode = plain)
    {
        return error_code(mode);
    }
}
#endif
