//  Copyright (c) 2007-2013 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_PERFORMANCE_COUNTERS_COUNTERS_FWD_HPP)
#define HPX_PERFORMANCE_COUNTERS_COUNTERS_FWD_HPP

#include <hpx/exception_fwd.hpp>
#include <hpx/runtime/naming/name.hpp>
#include <hpx/util/function.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace performance_counters
{
    // default version of performance counter structures
    #define HPX_PERFORMANCE_COUNTER_V1 0x01000000

    struct counter_info;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief This declares the type of a function, which will be
    ///        called by HPX whenever a new performance counter instance of a
    ///        particular type needs to be created.
    typedef hpx::util::function_nonser<
        naming::gid_type(counter_info const&, error_code&)>
        create_counter_func;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief This declares a type of a function, which will be passed to
    ///        a \a discover_counters_func in order to be called for each
    ///        discovered performance counter instance.
    typedef hpx::util::function_nonser<
        bool(counter_info const&, error_code&)>
        discover_counter_func;

    enum discover_counters_mode
    {
        discover_counters_minimal,
        discover_counters_full      ///< fully expand all wild cards
    };

    /// \brief This declares the type of a function, which will be called by
    ///        HPX whenever it needs to discover all performance counter
    ///        instances of a particular type.
    typedef hpx::util::function_nonser<
        bool(counter_info const&, discover_counter_func const&,
            discover_counters_mode, error_code&)>
        discover_counters_func;
}}

#endif
