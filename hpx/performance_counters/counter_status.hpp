//  Copyright (c) 2007-2013 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_PERFORMANCE_COUNTERS_COUNTER_STATUS_HPP)
#define HPX_PERFORMANCE_COUNTERS_COUNTER_STATUS_HPP

namespace hpx { namespace performance_counters
{

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Status and error codes used by the functions related to
    ///        performance counters.
    enum counter_status
    {
        status_valid_data,      ///< No error occurred, data is valid
        status_new_data,        ///< Data is valid and different from last call
        status_invalid_data,    ///< Some error occurred, data is not value
        status_already_defined, ///< The type or instance already has been defined
        status_counter_unknown, ///< The counter instance is unknown
        status_counter_type_unknown,  ///< The counter type is unknown
        status_generic_error    ///< A unknown error occurred
    };

    inline bool status_is_valid(counter_status s)
    {
        return s == status_valid_data || s == status_new_data;
    }
}}

#endif
