//  Copyright (c) 2007-2014 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//  Copyright (c)      2011 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file action_serialization_data.hpp

#if !defined(HPX_RUNTIME_ACTIONS_ACTION_SERIALIZATION_DATA_HPP)
#define HPX_RUNTIME_ACTIONS_ACTION_SERIALIZATION_DATA_HPP

#include <hpx/config.hpp>
#include <hpx/runtime/threads/thread_enums.hpp>
#include <hpx/traits/is_bitwise_serializable.hpp>

#include <boost/cstdint.hpp>

/// \cond NOINTERNAL
namespace hpx { namespace actions { namespace detail
{
    struct action_serialization_data
    {
        action_serialization_data()
          : parent_locality_(naming::invalid_locality_id)
          , parent_id_(static_cast<boost::uint64_t>(-1))
          , parent_phase_(0)
          , priority_(static_cast<threads::thread_priority>(0))
          , stacksize_(static_cast<threads::thread_stacksize>(0))
        {}

        action_serialization_data(boost::uint32_t parent_locality,
                boost::uint64_t parent_id,
                boost::uint64_t parent_phase,
                threads::thread_priority priority,
                threads::thread_stacksize stacksize)
          : parent_locality_(parent_locality)
          , parent_id_(parent_id)
          , parent_phase_(parent_phase)
          , priority_(priority)
          , stacksize_(stacksize)
        {}

        boost::uint32_t parent_locality_;
        boost::uint64_t parent_id_;
        boost::uint64_t parent_phase_;
        threads::thread_priority priority_;
        threads::thread_stacksize stacksize_;

        template <class Archive>
        void serialize(Archive& ar, unsigned)
        {
            ar & parent_id_ & parent_phase_ & parent_locality_
               & priority_ & stacksize_;
        }
    };
}}}

namespace hpx { namespace traits
{
    template <>
    struct is_bitwise_serializable<
            hpx::actions::detail::action_serialization_data>
       : boost::mpl::true_
    {};
}}

/// \endcond

#endif
