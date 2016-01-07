//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file expect_exception.hpp

#if !defined(HPX_EXCEPTION_EXPECT_EXCEPTION_HPP)
#define HPX_EXCEPTION_EXPECT_EXCEPTION_HPP

#include <hpx/config.hpp>

namespace hpx
{
    // \cond NOINTERNAL
    // For testing purposes we sometime expect to see exceptions, allow those
    // to go through without attaching a debugger.
    //
    // This should be used carefully as it disables the possible attaching of
    // a debugger for all exceptions, not only the expected ones.
    HPX_EXPORT bool expect_exception(bool flag = true);
    /// \endcond
}

#endif
