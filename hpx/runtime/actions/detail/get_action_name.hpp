//  Copyright (c) 2007-2014 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//  Copyright (c)      2011 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file get_action_name.hpp

#if !defined(HPX_RUNTIME_ACTIONS_GET_ACTION_NAME_HPP)
#define HPX_RUNTIME_ACTIONS_GET_ACTION_NAME_HPP

#include <hpx/config.hpp>

#include <hpx/traits/needs_automatic_registration.hpp>
#include <hpx/util/demangle_helper.hpp>

/// \cond NOINTERNAL
namespace hpx { namespace actions { namespace detail
{
    template <typename Action>
    char const* get_action_name()
#ifndef HPX_HAVE_AUTOMATIC_SERIALIZATION_REGISTRATION
    ;
#else
    {
        /// If you encounter this assert while compiling code, that means that
        /// you have a HPX_REGISTER_ACTION macro somewhere in a source file,
        /// but the header in which the action is defined misses a
        /// HPX_REGISTER_ACTION_DECLARATION
        static_assert(
            traits::needs_automatic_registration<Action>::value,
            "HPX_REGISTER_ACTION_DECLARATION missing");
        return util::type_id<Action>::typeid_.type_id();
    }
#endif

}}}
/// \endcond

#endif
