//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file get_category.hpp

#if !defined(HPX_EXCEPTION_GET_CATEGORY_HPP)
#define HPX_EXCEPTION_GET_CATEGORY_HPP

#include <hpx/config.hpp>
#include <hpx/error.hpp>
#include <hpx/exception/throwmode.hpp>
#include <hpx/exception/detail/hpx_category.hpp>

#include <boost/system/error_code.hpp>

namespace hpx {
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns generic HPX error category used for new errors.
    inline boost::system::error_category const& get_hpx_category()
    {
        static detail::hpx_category instance;
        return instance;
    }

    /// \brief Returns generic HPX error category used for errors re-thrown
    ///        after the exception has been de-serialized.
    inline boost::system::error_category const& get_hpx_rethrow_category()
    {
        static detail::hpx_category_rethrow instance;
        return instance;
    }

    /// \cond NOINTERNAL
    inline boost::system::error_category const&
    get_lightweight_hpx_category()
    {
        static detail::lightweight_hpx_category instance;
        return instance;
    }

    inline boost::system::error_category const& get_hpx_category(throwmode mode)
    {
        switch(mode) {
        case rethrow:
            return get_hpx_rethrow_category();

        case lightweight:
        case lightweight_rethrow:
            return get_lightweight_hpx_category();

        case plain:
        default:
            break;
        }
        return get_hpx_category();
    }

    inline boost::system::error_code
    make_system_error_code(error e, throwmode mode = plain)
    {
        return boost::system::error_code(
            static_cast<int>(e), get_hpx_category(mode));
    }

    ///////////////////////////////////////////////////////////////////////////
    inline boost::system::error_condition
    make_error_condition(error e, throwmode mode)
    {
        return boost::system::error_condition(
            static_cast<int>(e), get_hpx_category(mode));
    }
    /// \endcond
}

#endif
