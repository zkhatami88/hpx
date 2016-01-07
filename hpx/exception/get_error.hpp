//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file get_error.hpp

#if !defined(HPX_EXCEPTION_GET_ERROR_HPP)
#define HPX_EXCEPTION_GET_ERROR_HPP

#include <hpx/config.hpp>
#include <hpx/exception_fwd.hpp>
#include <hpx/error.hpp>

namespace hpx {
    /// \brief Return the error of the exception.
    ///
    /// The function \a hpx::get_error can be used to extract the
    /// diagnostic information element representing the error value code as
    /// stored in the given exception instance.
    ///
    /// \param e    The parameter \p e will be inspected for the requested
    ///             diagnostic information elements which have been stored at
    ///             the point where the exception was thrown. This parameter
    ///             can be one of the following types: \a hpx::exception,
    ///             \a hpx::error_code, or \a boost::exception_ptr.
    ///
    /// \returns    The error value code of the locality where the exception was
    ///             thrown. If the exception instance does not hold
    ///             this information, the function will return
    ///             \a hpx::naming#invalid_locality_id.
    ///
    /// \throws     nothing
    ///
    /// \see        \a hpx::diagnostic_information(), \a hpx::get_error_host_name(),
    ///             \a hpx::get_error_process_id(), \a hpx::get_error_function_name(),
    ///             \a hpx::get_error_file_name(), \a hpx::get_error_line_number(),
    ///             \a hpx::get_error_os_thread(), \a hpx::get_error_thread_id(),
    ///             \a hpx::get_error_thread_description(),
    ///             \a hpx::get_error_backtrace(), \a hpx::get_error_env(),
    ///             \a hpx::get_error_what(), \a hpx::get_error_config(),
    ///             \a hpx::get_error_state()
    ///
    HPX_EXPORT error get_error(hpx::exception const& e);

    /// \copydoc get_error(hpx::exception const& e)
    HPX_EXPORT error get_error(hpx::error_code const& e);

    /// \cond NOINTERNAL
    HPX_EXPORT error get_error(boost::exception_ptr const& e);
    /// \endcond
}

#endif
