//  Copyright (c) 2007-2013 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_PERFORMANCE_COUNTERS_MAR_01_2009_0134PM)
#define HPX_PERFORMANCE_COUNTERS_MAR_01_2009_0134PM

#include <hpx/config.hpp>
#include <hpx/performance_counters/counters_fwd.hpp>
#include <hpx/performance_counters/counter_status.hpp>
#include <hpx/performance_counters/counter_type.hpp>
#include <hpx/runtime/naming/name.hpp>
#include <hpx/runtime/serialization/serialize.hpp>
#include <hpx/runtime/serialization/base_object.hpp>
#include <hpx/lcos/future.hpp>
#include <hpx/util/function.hpp>

#include <boost/cstdint.hpp>

#include <string>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace performance_counters
{
    ///////////////////////////////////////////////////////////////////////////
    char const counter_prefix[] = "/counters";

    ///////////////////////////////////////////////////////////////////////////
    inline std::string& ensure_counter_prefix(std::string& name)
    {
        if (name.find(counter_prefix) != 0)
            name = counter_prefix + name;
        return name;
    }

    inline std::string ensure_counter_prefix(std::string const& counter) //-V659
    {
        std::string name(counter);
        return ensure_counter_prefix(name);
    }

    inline std::string& remove_counter_prefix(std::string& name)
    {
        if (name.find(counter_prefix) == 0)
            name = name.substr(sizeof(counter_prefix)-1);
        return name;
    }

    inline std::string remove_counter_prefix(std::string const& counter) //-V659
    {
        std::string name(counter);
        return remove_counter_prefix(name);
    }

    /// \brief Return the canonical counter instance name from a given full
    ///        instance name
    HPX_API_EXPORT counter_status get_counter_name(
        std::string const& name, std::string& countername,
        error_code& ec = throws);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Complement the counter info if parent instance name is missing
    HPX_API_EXPORT counter_status complement_counter_info(counter_info& info,
        counter_info const& type_info, error_code& ec = throws);

    HPX_API_EXPORT counter_status complement_counter_info(counter_info& info,
        error_code& ec = throws);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Call the supplied function for each registered counter type
    HPX_API_EXPORT counter_status discover_counter_types(
        discover_counter_func const& discover_counter,
        discover_counters_mode mode = discover_counters_minimal,
        error_code& ec = throws);

    /// \brief Return a list of all available counter descriptions.
    HPX_API_EXPORT counter_status discover_counter_types(
        std::vector<counter_info>& counters,
        discover_counters_mode mode = discover_counters_minimal,
        error_code& ec = throws);

    /// \brief Call the supplied function for the given registered counter type.
    HPX_API_EXPORT counter_status discover_counter_type(
        std::string const& name,
        discover_counter_func const& discover_counter,
        discover_counters_mode mode = discover_counters_minimal,
        error_code& ec = throws);

    HPX_API_EXPORT counter_status discover_counter_type(
        counter_info const& info,
        discover_counter_func const& discover_counter,
        discover_counters_mode mode = discover_counters_minimal,
        error_code& ec = throws);

    /// \brief Return a list of matching counter descriptions for the given
    ///        registered counter type.
    HPX_API_EXPORT counter_status discover_counter_type(
        std::string const& name, std::vector<counter_info>& counters,
        discover_counters_mode mode = discover_counters_minimal,
        error_code& ec = throws);

    HPX_API_EXPORT counter_status discover_counter_type(
        counter_info const& info, std::vector<counter_info>& counters,
        discover_counters_mode mode = discover_counters_minimal,
        error_code& ec = throws);

    /// \brief call the supplied function will all expanded versions of the
    /// supplied counter info.
    ///
    /// This function expands all locality#* and worker-thread#* wild
    /// cards only.
    HPX_API_EXPORT bool expand_counter_info(counter_info const&,
        discover_counter_func const&, error_code&);

    /// \brief Remove an existing counter type from the (local) registry
    ///
    /// \note This doesn't remove existing counters of this type, it just
    ///       inhibits defining new counters using this type.
    HPX_API_EXPORT counter_status remove_counter_type(
        counter_info const& info, error_code& ec = throws);

    /// \brief Retrieve the counter type for the given counter name from the
    ///        (local) registry
    HPX_API_EXPORT counter_status get_counter_type(std::string const& name,
        counter_info& info, error_code& ec = throws);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the global id of an existing performance counter, if the
    ///        counter does not exist yet, the function attempts to create the
    ///        counter based on the given counter name.
    HPX_API_EXPORT lcos::future<naming::id_type>
        get_counter_async(std::string const& name, error_code& ec = throws);

    HPX_API_EXPORT naming::id_type get_counter(std::string const& name,
        error_code& ec = throws);

    /// \brief Get the global id of an existing performance counter, if the
    ///        counter does not exist yet, the function attempts to create the
    ///        counter based on the given counter info.
    HPX_API_EXPORT lcos::future<naming::id_type>
        get_counter_async(counter_info const& info, error_code& ec = throws);

    HPX_API_EXPORT naming::id_type get_counter(counter_info const& info,
        error_code& ec = throws);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Retrieve the meta data specific for the given counter instance
    HPX_API_EXPORT void get_counter_infos(counter_info const& info,
        counter_type& type, std::string& helptext, boost::uint32_t& version,
        error_code& ec = throws);

    /// \brief Retrieve the meta data specific for the given counter instance
    HPX_API_EXPORT void get_counter_infos(std::string name, counter_type& type,
        std::string& helptext, boost::uint32_t& version, error_code& ec = throws);

    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        /// \brief Add an existing performance counter instance to the registry
        HPX_API_EXPORT counter_status add_counter(naming::id_type const& id,
            counter_info const& info, error_code& ec = throws);

        /// \brief Remove an existing performance counter instance with the
        ///        given id (as returned from \a create_counter)
        HPX_API_EXPORT counter_status remove_counter(
            counter_info const& info, naming::id_type const& id,
            error_code& ec = throws);

        ///////////////////////////////////////////////////////////////////////
        // Helper function for creating counters encapsulating a function
        // returning the counter value.
        naming::gid_type create_raw_counter(counter_info const&,
            hpx::util::function_nonser<boost::int64_t()> const&, error_code&);

        // Helper function for creating counters encapsulating a function
        // returning the counter value.
        naming::gid_type create_raw_counter(counter_info const&,
            hpx::util::function_nonser<boost::int64_t(bool)> const&, error_code&);

        // Helper function for creating a new performance counter instance
        // based on a given counter value.
        naming::gid_type create_raw_counter_value(counter_info const&,
            boost::int64_t*, error_code&);

        // Creation function for aggregating performance counters; to be
        // registered with the counter types.
        naming::gid_type statistics_counter_creator(counter_info const&,
            error_code&);

        // Creation function for aggregating performance counters; to be
        // registered with the counter types.
        naming::gid_type arithmetics_counter_creator(counter_info const&,
            error_code&);

        // Creation function for uptime counters.
        naming::gid_type uptime_counter_creator(counter_info const&,
            error_code&);

        // Creation function for instance counters.
        naming::gid_type component_instance_counter_creator(counter_info const&,
            error_code&);

        // \brief Create a new statistics performance counter instance based on
        //        the given base counter name and given base time interval
        //        (milliseconds).
        naming::gid_type create_statistics_counter(
            counter_info const& info, std::string const& base_counter_name,
            std::vector<boost::int64_t> const& parameters,
            error_code& ec = throws);

        // \brief Create a new arithmetics performance counter instance based on
        //        the given base counter names
        naming::gid_type create_arithmetics_counter(counter_info const& info,
            std::vector<std::string> const& base_counter_names,
            error_code& ec = throws);

        // \brief Create a new performance counter instance based on given
        //        counter info
        naming::gid_type create_counter(counter_info const& info,
            error_code& ec = throws);

        // \brief Create an arbitrary counter on this locality
        naming::gid_type create_counter_local(counter_info const& info);
    }
}}

#endif

