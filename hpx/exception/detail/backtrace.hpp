//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_EXCEPTION_DETAIL_BACKTRACE_HPP)
#define HPX_EXCEPTION_DETAIL_BACKTRACE_HPP

#include <hpx/config.hpp>

#include <string>

namespace hpx { namespace detail {
    // If backtrace support is enabled, this function returns the current
    // stack backtrace, otherwise it will return an empty string.
    HPX_EXPORT std::string backtrace(
        std::size_t frames = HPX_THREAD_BACKTRACE_ON_SUSPENSION_DEPTH);
    HPX_EXPORT std::string backtrace_direct(
        std::size_t frames = HPX_THREAD_BACKTRACE_ON_SUSPENSION_DEPTH);
}}

#endif
