//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file get_error_state.hpp

#if !defined(HPX_EXCEPTION_GET_ERROR_STATE_HPP)
#define HPX_EXCEPTION_GET_ERROR_STATE_HPP

#include <hpx/config.hpp>
#include <hpx/exception_fwd.hpp>

#include <string>

namespace hpx {
    /// \brief Return the HPX runtime state information at which the exception
    ///        was thrown.
    ///
    /// The function \a hpx::get_error_state can be used to extract the
    /// HPX runtime state information element representing the state the
    /// runtime system is currently in as stored in the given exception
    /// instance.
    ///
    /// \returns    The point runtime state at the point at which the exception
    ///             was thrown. If the exception instance does not hold
    ///             this information, the function will return an empty string.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, \a boost::exception, or
    ///             \a boost::exception_ptr.
    ///
    /// \throws     std#bad_alloc (if one of the required allocations fails)
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_thread_description()
    ///
    HPX_EXPORT std::string get_error_state(hpx::exception const& e);

    /// \copydoc get_error_state(hpx::exception const& e)
    HPX_EXPORT std::string get_error_state(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT std::string get_error_state(boost::exception const& e);
    HPX_EXPORT std::string get_error_state(boost::exception_ptr const& e);
    /// \endcond
}

#endif
