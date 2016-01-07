//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_EXCEPTION_DETAIL_GET_EXECUTION_ENVIRONMENT_HPP)
#define HPX_EXCEPTION_DETAIL_GET_EXECUTION_ENVIRONMENT_HPP

#include <hpx/config.hpp>

#include <string>

namespace hpx { namespace detail {
    // Portably extract the current execution environment
    HPX_EXPORT std::string get_execution_environment();
}}

#endif
