//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_EXCEPTION_DETAIL_CONSTRUCT_EXCEPTION_HPP)
#define HPX_EXCEPTION_DETAIL_CONSTRUCT_EXCEPTION_HPP

#include <hpx/config.hpp>
#include <hpx/exception_fwd.hpp>

#include <boost/cstdint.hpp>

#include <string>

namespace hpx { namespace detail {
    // construct an exception, internal helper
    template <typename Exception>
    HPX_EXPORT boost::exception_ptr
        construct_exception(Exception const& e,
            std::string const& func, std::string const& file, long line,
            std::string const& back_trace = "", boost::uint32_t node = 0,
            std::string const& hostname = "", boost::int64_t pid = -1,
            std::size_t shepherd = ~0, std::size_t thread_id = 0,
            std::string const& thread_name = "",
            std::string const& env = "", std::string const& config = "",
            std::string const& state = "", std::string const& auxinfo = "");

    template <typename Exception>
    HPX_EXPORT boost::exception_ptr
        construct_lightweight_exception(Exception const& e);
}}

#endif
