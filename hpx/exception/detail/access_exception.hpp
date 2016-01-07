//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_EXCEPTION_DETAIL_ACCESS_EXCEPTION_HPP)
#define HPX_EXCEPTION_DETAIL_ACCESS_EXCEPTION_HPP

#include <hpx/config.hpp>
#include <hpx/exception_fwd.hpp>

#include <boost/exception_ptr.hpp>

namespace hpx { namespace detail {
    ///////////////////////////////////////////////////////////////////////
    HPX_EXPORT boost::exception_ptr access_exception(error_code const&);
}}

#endif

