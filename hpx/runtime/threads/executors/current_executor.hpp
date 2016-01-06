//  Copyright (c) 2007-2015 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_RUNTIME_THREADS_EXECUTORS_CURRENT_EXECUTOR_JAN_11_2013_0831PM)
#define HPX_RUNTIME_THREADS_EXECUTORS_CURRENT_EXECUTOR_JAN_11_2013_0831PM

#include <hpx/config.hpp>
#include <hpx/state.hpp>
#include <hpx/runtime/threads_fwd.hpp>
#include <hpx/runtime/threads/thread_executor.hpp>

#include <hpx/config/warnings_prefix.hpp>

namespace hpx { namespace threads { namespace executors
{
    ///////////////////////////////////////////////////////////////////////////
    struct HPX_EXPORT current_executor : public scheduled_executor
    {
        current_executor();
        explicit current_executor(policies::scheduler_base* scheduler);

        hpx::state get_state() const;
    };
}}}

#include <hpx/config/warnings_suffix.hpp>

#endif

