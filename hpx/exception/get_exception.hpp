//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file get_error_what.hpp

#if !defined(HPX_EXCEPTION_GET_EXCEPTION_HPP)
#define HPX_EXCEPTION_GET_EXCEPTION_HPP

#include <hpx/config.hpp>
#include <hpx/exception_fwd.hpp>

#include <string>

namespace hpx {
    /// \cond NODETAIL
    namespace detail
    {
        // main function for throwing exceptions
        template <typename Exception>
        HPX_EXPORT boost::exception_ptr
            get_exception(Exception const& e,
                std::string const& func = "<unknown>",
                std::string const& file = "<unknown>",
                long line = -1,
                std::string const& auxinfo = "");
    }
    /// \endcond
}

#endif
