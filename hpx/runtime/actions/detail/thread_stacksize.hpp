//  Copyright (c) 2007-2014 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//  Copyright (c)      2011 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file thread_stacksize.hpp

#if !defined(HPX_RUNTIME_ACTIONS_THREAD_STACKSIZE_HPP)
#define HPX_RUNTIME_ACTIONS_THREAD_STACKSIZE_HPP

#include <hpx/config.hpp>
#include <hpx/runtime/threads/thread_enums.hpp>

/// \cond NOINTERNAL
namespace hpx { namespace actions { namespace detail
{
    ///////////////////////////////////////////////////////////////////////
    // Figure out what stacksize the action has to be be associated with
    // A dynamically specified default stacksize results in using the static
    // Stacksize.
    template <threads::thread_stacksize Stacksize>
    struct thread_stacksize
    {
        static threads::thread_stacksize
        call(threads::thread_stacksize stacksize)
        {
            if (stacksize == threads::thread_stacksize_default)
                return Stacksize;
            return stacksize;
        }
    };

    // If the static Stacksize is default, a dynamically specified default
    // stacksize results in using the normal stacksize.
    template <>
    struct thread_stacksize<threads::thread_stacksize_default>
    {
        static threads::thread_stacksize
        call(threads::thread_stacksize stacksize)
        {
            if (stacksize == threads::thread_stacksize_default)
                return threads::thread_stacksize_minimal;
            return stacksize;
        }
    };
}}}
/// \endcond

#endif
