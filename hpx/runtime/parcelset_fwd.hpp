//  Copyright (c) 2007-2014 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file parcelset_fwd.hpp

#ifndef HPX_RUNTIME_PARCELSET_FWD_HPP
#define HPX_RUNTIME_PARCELSET_FWD_HPP

#include <hpx/config/export_definitions.hpp>
#include <hpx/exception_fwd.hpp>
#include <hpx/runtime/threads/thread_enums.hpp>
#include <hpx/util/function.hpp>

namespace hpx {
    ///////////////////////////////////////////////////////////////////////////
    /// \namespace parcelset
    namespace parcelset
    {
        class HPX_API_EXPORT locality;

        class HPX_API_EXPORT parcel;
        class HPX_API_EXPORT parcelport;
        class HPX_API_EXPORT parcelhandler;

        namespace policies
        {
            struct message_handler;
        }

        HPX_API_EXPORT policies::message_handler* get_message_handler(
            parcelhandler* ph, char const* name, char const* type,
            std::size_t num, std::size_t interval, locality const& l,
            error_code& ec = throws);

        HPX_API_EXPORT bool do_background_work(std::size_t num_thread = 0);

        typedef util::function_nonser<
            void(boost::system::error_code const&, parcel const&)
        > write_handler_type;

        typedef util::function_nonser<
            void(parcelport& pp, boost::shared_ptr<std::vector<char> >,
                 threads::thread_priority)
        > read_handler_type;

        /// Return the given connection cache statistic
        enum connection_cache_statistics_type
        {
            connection_cache_insertions = 0,
            connection_cache_evictions = 1,
            connection_cache_hits = 2,
            connection_cache_misses = 3,
            connection_cache_reclaims = 4
        };

        class locality;

        typedef std::map<std::string, locality> endpoints_type;

        std::ostream& operator<< (std::ostream& os, endpoints_type const& endpoints);
    }
}

#endif
