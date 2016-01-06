//  Copyright (c) 2007-2014 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//  Copyright (c)      2011 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file thread_priority.hpp

#if !defined(HPX_RUNTIME_ACTIONS_THREAD_PRIORITY_HPP)
#define HPX_RUNTIME_ACTIONS_THREAD_PRIORITY_HPP

#include <hpx/config.hpp>
#include <hpx/runtime/threads/thread_enums.hpp>

/// \cond NOINTERNAL
namespace hpx { namespace actions { namespace detail
{
    ///////////////////////////////////////////////////////////////////////
    // Figure out what priority the action has to be be associated with
    // A dynamically specified default priority results in using the static
    // Priority.
    template <threads::thread_priority Priority>
    struct thread_priority
    {
        static threads::thread_priority
        call(threads::thread_priority priority)
        {
            if (priority == threads::thread_priority_default)
                return Priority;
            return priority;
        }
    };

    // If the static Priority is default, a dynamically specified default
    // priority results in using the normal priority.
    template <>
    struct thread_priority<threads::thread_priority_default>
    {
        static threads::thread_priority
        call(threads::thread_priority priority)
        {
            if (priority == threads::thread_priority_default)
                return threads::thread_priority_normal;
            return priority;
        }
    };
}}}
/// \endcond

#endif
