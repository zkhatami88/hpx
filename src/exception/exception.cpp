//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/exception/exception.hpp>
#include <hpx/exception/get_category.hpp>
#include <hpx/util/assert.hpp>
#include <hpx/util/logging.hpp>
#include <hpx/util/unused.hpp>

namespace hpx {
    exception::exception(error e)
      : boost::system::system_error(make_error_code(e, plain))
    {
        HPX_ASSERT(e >= success && e < last_error);
        LERR_(error) << "created exception: " << this->what();
    }

    exception::exception(boost::system::system_error const& e)
      : boost::system::system_error(e)
    {
        LERR_(error) << "created exception: " << this->what();
    }

    exception::exception(error e, char const* msg, throwmode mode)
      : boost::system::system_error(make_system_error_code(e, mode), msg)
    {
        HPX_ASSERT(e >= success && e < last_error);
        LERR_(error) << "created exception: " << this->what();
    }

    exception::exception(error e, std::string const& msg, throwmode mode)
      : boost::system::system_error(make_system_error_code(e, mode), msg)
    {
        HPX_ASSERT(e >= success && e < last_error);
        LERR_(error) << "created exception: " << this->what();
    }

    error exception::get_error() const HPX_NOEXCEPT_OR_NOTHROW
    {
        return static_cast<error>(
            this->boost::system::system_error::code().value());
    }

    error_code exception::get_error_code(throwmode mode) const HPX_NOEXCEPT_OR_NOTHROW
    {
        HPX_UNUSED(mode);
        return error_code(this->boost::system::system_error::code().value(),
            *this);
    }
}
