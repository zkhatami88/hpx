//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file throwmode.hpp

#if !defined(HPX_EXCEPTION_THROW_MODE_HPP)
#define HPX_EXCEPTION_THROW_MODE_HPP

#include <hpx/config.hpp>
#include <hpx/exception_fwd.hpp>

namespace hpx {
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Encode error category for new error_code.
    enum throwmode
    {
        plain = 0,
        rethrow = 1,
        lightweight = 0x80, // do not generate an exception for this error_code
        /// \cond NODETAIL
        lightweight_rethrow = lightweight | rethrow
        /// \endcond
    };

    HPX_EXPORT throwmode get_throwmode(error_code const & ec);
    HPX_EXPORT throwmode get_rethrowmode(error_code const & ec);
}

#endif
