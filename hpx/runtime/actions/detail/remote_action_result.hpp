//  Copyright (c) 2007-2014 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//  Copyright (c)      2011 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file remote_action_result.hpp

#if !defined(HPX_RUNTIME_ACTIONS_REMOTE_ACTION_RESULT_HPP)
#define HPX_RUNTIME_ACTIONS_REMOTE_ACTION_RESULT_HPP

#include <hpx/config.hpp>

#include <hpx/lcos_fwd.hpp>
#include <hpx/util/unused.hpp>

/// \cond NOINTERNAL
namespace hpx { namespace actions { namespace detail
{
    ///////////////////////////////////////////////////////////////////////
    // If an action returns a future, we need to do special things
    template <typename Result>
    struct remote_action_result
    {
        typedef Result type;
    };

    template <>
    struct remote_action_result<void>
    {
        typedef util::unused_type type;
    };

    template <typename Result>
    struct remote_action_result<lcos::future<Result> >
    {
        typedef Result type;
    };

    template <>
    struct remote_action_result<lcos::future<void> >
    {
        typedef hpx::util::unused_type type;
    };

    template <typename Result>
    struct remote_action_result<lcos::shared_future<Result> >
    {
        typedef Result type;
    };

    template <>
    struct remote_action_result<lcos::shared_future<void> >
    {
        typedef hpx::util::unused_type type;
    };
}}}
/// \endcond

#endif
