//  Copyright (c) 2007-2013 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_PERFORMANCE_COUNTERS_COUNTER_TYPE_PATH_ELEMENTS_HPP)
#define HPX_PERFORMANCE_COUNTERS_COUNTER_TYPE_PATH_ELEMENTS_HPP

#include <hpx/exception_fwd.hpp>
#include <hpx/performance_counters/counter_status.hpp>
#include <hpx/runtime/serialization/serialization_fwd.hpp>

#include <string>

namespace hpx { namespace performance_counters
{
    ///////////////////////////////////////////////////////////////////////////
    /// A counter_type_path_elements holds the elements of a full name for a
    /// counter type. Generally, a full name of a counter type has the
    /// structure:
    ///
    ///    /objectname/countername
    ///
    /// i.e.
    ///    /queue/length
    ///
    struct counter_type_path_elements
    {
        counter_type_path_elements()
        {}

        counter_type_path_elements(std::string const& objectname,
                std::string const& countername, std::string const& parameters)
          : objectname_(objectname),
            countername_(countername),
            parameters_(parameters)
        {}

        std::string objectname_;          ///< the name of the performance object
        std::string countername_;         ///< contains the counter name
        std::string parameters_;          ///< optional parameters for the
                                          ///< counter instance

    protected:
        // serialization support
        friend class hpx::serialization::access;

        template<class Archive>
        void serialize(Archive& ar, const unsigned int)
        {
            ar & objectname_ & countername_ & parameters_;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create a full name of a counter type from the contents of the
    ///        given \a counter_type_path_elements instance.The generated
    ///        counter type name will not contain any parameters.
    HPX_API_EXPORT counter_status get_counter_type_name(
        counter_type_path_elements const& path, std::string& result,
        error_code& ec = throws);

    /// \brief Create a full name of a counter type from the contents of the
    ///        given \a counter_type_path_elements instance. The generated
    ///        counter type name will contain all parameters.
    HPX_API_EXPORT counter_status get_full_counter_type_name(
        counter_type_path_elements const& path, std::string& result,
        error_code& ec = throws);

    /// \brief Fill the given \a counter_type_path_elements instance from the
    ///        given full name of a counter type
    HPX_API_EXPORT counter_status get_counter_type_path_elements(
        std::string const& name, counter_type_path_elements& path,
        error_code& ec = throws);
}}

#endif
