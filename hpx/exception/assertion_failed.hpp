//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_EXCEPTION_ASSERTION_FAILED_HPP)
#define HPX_EXCEPTION_ASSERTION_FAILED_HPP

#include <hpx/config.hpp>

namespace hpx {
    namespace detail {
        // HPX_ASSERT handler
        HPX_ATTRIBUTE_NORETURN HPX_EXPORT
        void assertion_failed(char const* expr, char const* function,
            char const* file, long line);

        // HPX_ASSERT_MSG handler
        HPX_ATTRIBUTE_NORETURN HPX_EXPORT
        void assertion_failed_msg(char const* msg, char const* expr,
            char const* function, char const* file, long line);
    }

    // forwarder for HPX_ASSERT handler
    inline void assertion_failed(char const* expr, char const* function,
        char const* file, long line)
    {
        hpx::detail::assertion_failed(expr, function, file, line);
    }

    // forwarder for HPX_ASSERT_MSG handler
    inline void assertion_failed_msg(char const* msg, char const* expr,
        char const* function, char const* file, long line)
    {
        hpx::detail::assertion_failed_msg(msg, expr, function, file, line);
    }
}

#endif
