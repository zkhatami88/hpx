//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_EXCEPTION_DETAIL_COMMAND_LINE_ERROR_HPP)
#define HPX_EXCEPTION_DETAIL_COMMAND_LINE_ERROR_HPP

#include <stdexcept>
#include <string>

namespace hpx { namespace detail {
    ///////////////////////////////////////////////////////////////////////
    struct command_line_error : std::logic_error
    {
        explicit command_line_error(char const* msg)
          : std::logic_error(msg)
        {}

        explicit command_line_error(std::string const& msg)
          : std::logic_error(msg)
        {}
    };
}}

#endif

