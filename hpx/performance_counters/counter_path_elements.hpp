//  Copyright (c) 2007-2013 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_PERFORMANCE_COUNTERS_COUNTER_PATH_ELEMENTS_HPP)
#define HPX_PERFORMANCE_COUNTERS_COUNTER_PATH_ELEMENTS_HPP

#include <hpx/exception_fwd.hpp>
#include <hpx/runtime/serialization/serialization_fwd.hpp>
#include <hpx/runtime/serialization/base_object.hpp>
#include <hpx/performance_counters/counter_type_path_elements.hpp>

#include <boost/cstdint.hpp>

#include <string>

namespace hpx { namespace performance_counters
{
    ///////////////////////////////////////////////////////////////////////////
    /// A counter_path_elements holds the elements of a full name for a counter
    /// instance. Generally, a full name of a counter instance has the
    /// structure:
    ///
    ///    /objectname{parentinstancename#parentindex/instancename#instanceindex}
    ///      /countername#parameters
    ///
    /// i.e.
    ///    /queue{localityprefix/thread#2}/length
    ///
    struct counter_path_elements : counter_type_path_elements
    {
        typedef counter_type_path_elements base_type;

        counter_path_elements()
          : parentinstanceindex_(-1), instanceindex_(-1),
            parentinstance_is_basename_(false)
        {}

        counter_path_elements(std::string const& objectname,
                std::string const& countername, std::string const& parameters,
                std::string const& parentname, std::string const& instancename,
                boost::int64_t parentindex = -1, boost::int64_t instanceindex = -1,
                bool parentinstance_is_basename = false)
          : counter_type_path_elements(objectname, countername, parameters),
            parentinstancename_(parentname), instancename_(instancename),
            parentinstanceindex_(parentindex), instanceindex_(instanceindex),
            parentinstance_is_basename_(parentinstance_is_basename)
        {}

        std::string parentinstancename_;  ///< the name of the parent instance
        std::string instancename_;        ///< the name of the object instance
        boost::int64_t parentinstanceindex_;    ///< the parent instance index
        boost::int64_t instanceindex_;    ///< the instance index
        bool parentinstance_is_basename_; ///< the parentinstancename_
                                          ///member holds a base counter name

    private:
        // serialization support
        friend class hpx::serialization::access;

        template<class Archive>
        void serialize(Archive& ar, const unsigned int)
        {
            typedef counter_type_path_elements base_type;
            hpx::serialization::base_object_type<
              counter_path_elements, base_type> base =
                hpx::serialization::base_object<base_type>(*this);
            ar & base & parentinstancename_ & instancename_ &
                 parentinstanceindex_ & instanceindex_ &
                 parentinstance_is_basename_;
        }
    };

    /// \brief Create a full name of a counter from the contents of the given
    ///        \a counter_path_elements instance.
    HPX_API_EXPORT counter_status get_counter_name(
        counter_path_elements const& path, std::string& result,
        error_code& ec = throws);

    /// \brief Create a name of a counter instance from the contents of the
    ///        given \a counter_path_elements instance.
    HPX_API_EXPORT counter_status get_counter_instance_name(
        counter_path_elements const& path, std::string& result,
        error_code& ec = throws);

    /// \brief Fill the given \a counter_path_elements instance from the given
    ///        full name of a counter
    HPX_API_EXPORT counter_status get_counter_path_elements(
        std::string const& name, counter_path_elements& path,
        error_code& ec = throws);
}}

#endif
