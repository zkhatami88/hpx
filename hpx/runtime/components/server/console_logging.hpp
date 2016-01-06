//  Copyright (c) 2007-2012 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_COMPONENTS_CONSOLE_DEC_16_2008_0427PM)
#define HPX_COMPONENTS_CONSOLE_DEC_16_2008_0427PM

#include <hpx/config.hpp>
#include <hpx/runtime/actions/plain_action.hpp>
#include <hpx/runtime/components_fwd.hpp>
#include <hpx/util/tuple.hpp>

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace components
{
    typedef hpx::util::tuple<
        logging_destination, std::size_t, std::string
    > message_type;

    typedef std::vector<message_type> messages_type;
}}

//////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace components { namespace server
{
    ///////////////////////////////////////////////////////////////////////////
    // console logging happens here
    void console_logging(messages_type const&);


    HPX_DEFINE_PLAIN_DIRECT_ACTION(console_logging);
}}}

HPX_REGISTER_ACTION_DECLARATION(
    hpx::components::server::console_logging_action,
    console_logging_action)

#endif

