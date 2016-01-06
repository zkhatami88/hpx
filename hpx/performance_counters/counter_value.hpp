//  Copyright (c) 2007-2013 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_PERFORMANCE_COUNTERS_COUNTER_VALUE_HPP)
#define HPX_PERFORMANCE_COUNTERS_COUNTER_VALUE_HPP

#include <hpx/exception.hpp>
#include <hpx/performance_counters/counter_status.hpp>
#include <hpx/runtime/serialization/serialization_fwd.hpp>

#include <boost/cstdint.hpp>

namespace hpx { namespace performance_counters
{
    ///////////////////////////////////////////////////////////////////////////
    struct counter_value
    {
        counter_value(boost::int64_t value = 0, boost::int64_t scaling = 1,
                bool scale_inverse = false)
          : time_(), count_(0), value_(value), scaling_(scaling),
            status_(status_new_data),
            scale_inverse_(scale_inverse)
        {}

        boost::uint64_t time_;      ///< The local time when data was collected
        boost::uint64_t count_;     ///< The invocation counter for the data
        boost::int64_t value_;      ///< The current counter value
        boost::int64_t scaling_;    ///< The scaling of the current counter value
        counter_status status_;     ///< The status of the counter value
        bool scale_inverse_;        ///< If true, value_ needs to be divided by
                                    ///< scaling_, otherwise it has to be
                                    ///< multiplied.

        /// \brief Retrieve the 'real' value of the counter_value, converted to
        ///        the requested type \a T
        template <typename T>
        T get_value(error_code& ec = throws) const
        {
            if (!status_is_valid(status_)) {
                HPX_THROWS_IF(ec, invalid_status,
                    "counter_value::get_value<T>",
                    "counter value is in invalid status");
                return T();
            }

            T val = static_cast<T>(value_);

            if (scaling_ != 1) {
                if (scaling_ == 0) {
                    HPX_THROWS_IF(ec, uninitialized_value,
                        "counter_value::get_value<T>",
                        "scaling should not be zero");
                    return T();
                }

                // calculate and return the real counter value
                if (scale_inverse_)
                    return val / static_cast<T>(scaling_);

                return val * static_cast<T>(scaling_);
            }
            return val;
        }

    private:
        // serialization support
        friend class hpx::serialization::access;

        template<class Archive>
        void serialize(Archive& ar, const unsigned int)
        {
            ar & status_ & time_ & count_ & value_ & scaling_ & scale_inverse_;
        }
    };
}}

#endif
