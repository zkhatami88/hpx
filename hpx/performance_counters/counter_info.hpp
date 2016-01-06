//  Copyright (c) 2007-2013 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_PERFORMANCE_COUNTERS_COUNTER_INFO_HPP)
#define HPX_PERFORMANCE_COUNTERS_COUNTER_INFO_HPP

#include <hpx/performance_counters/counters_fwd.hpp>
#include <hpx/performance_counters/counter_status.hpp>
#include <hpx/performance_counters/counter_type.hpp>
#include <hpx/runtime/serialization/serialization_fwd.hpp>

#include <boost/cstdint.hpp>

#include <string>

namespace hpx { namespace performance_counters
{
    ///////////////////////////////////////////////////////////////////////////
    struct counter_info
    {
        counter_info(counter_type type = counter_raw)
          : type_(type), version_(HPX_PERFORMANCE_COUNTER_V1),
            status_(status_invalid_data)
        {}

        counter_info(std::string const& name)
          : type_(counter_raw), version_(HPX_PERFORMANCE_COUNTER_V1),
            status_(status_invalid_data), fullname_(name)
        {}

        counter_info(counter_type type, std::string const& name,
                std::string const& helptext = "",
                boost::uint32_t version = HPX_PERFORMANCE_COUNTER_V1,
                std::string const& uom = "")
          : type_(type), version_(version), status_(status_invalid_data),
            fullname_(name), helptext_(helptext), unit_of_measure_(uom)
        {}

        counter_type type_;         ///< The type of the described counter
        boost::uint32_t version_;   ///< The version of the described counter
                                    ///< using the 0xMMmmSSSS scheme
        counter_status status_;     ///< The status of the counter object
        std::string fullname_;      ///< The full name of this counter
        std::string helptext_;      ///< The full descriptive text for this counter
        std::string unit_of_measure_; ///< The unit of measure for this counter

    private:
        // serialization support
        friend class hpx::serialization::access;

        template<class Archive>
        void serialize(Archive& ar, const unsigned int)
        {
            ar & type_ & version_ & status_
               & fullname_ & helptext_
               & unit_of_measure_;
        }
    };

    ///////////////////////////////////////////////////////////////////////
    /// \brief Add a new performance counter type to the (local) registry
    HPX_API_EXPORT counter_status add_counter_type(counter_info const& info,
        create_counter_func const& create_counter,
        discover_counters_func const& discover_counters,
        error_code& ec = throws);

    inline counter_status add_counter_type(counter_info const& info,
        error_code& ec = throws)
    {
        return add_counter_type(info, create_counter_func(),
            discover_counters_func(), ec);
    }
}}

#endif

