//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_EXCEPTION_DETAIL_REPORT_EXCEPTION_HPP)
#define HPX_EXCEPTION_DETAIL_REPORT_EXCEPTION_HPP

#include <hpx/config.hpp>
#include <hpx/exception_fwd.hpp>

namespace hpx { namespace detail {
    // Report an early or late exception and locally abort execution. There
    // isn't anything more we could do.
    HPX_EXPORT void report_exception_and_terminate(boost::exception_ptr const&);
    HPX_EXPORT void report_exception_and_terminate(hpx::exception const&);

    // Report an early or late exception and locally exit execution. There
    // isn't anything more we could do. The exception will be re-thrown
    // from hpx::init
    HPX_EXPORT void report_exception_and_continue(boost::exception_ptr const&);
    HPX_EXPORT void report_exception_and_continue(hpx::exception const&);
}}

#endif
