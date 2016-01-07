//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/exception/error_code.hpp>
#include <hpx/exception/exception.hpp>
#include <hpx/exception/get_category.hpp>
#include <hpx/exception/get_error.hpp>
#include <hpx/exception/get_error_what.hpp>
#include <hpx/exception/get_exception.hpp>
#include <hpx/util/filesystem_compatibility.hpp>

#include <boost/current_function.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/filesystem/path.hpp>

namespace hpx {
    error_code::error_code(throwmode mode)
      : boost::system::error_code(make_system_error_code(success, mode))
    {}

    error_code::error_code(error e, throwmode mode)
      : boost::system::error_code(make_system_error_code(e, mode))
    {
        if (e != success && e != no_success && !(mode & lightweight))
            exception_ = detail::get_exception(hpx::exception(e, "", mode));
    }

    error_code::error_code(error e, char const* func,
            char const* file, long line, throwmode mode)
      : boost::system::error_code(make_system_error_code(e, mode))
    {
        if (e != success && e != no_success && !(mode & lightweight)) {
            boost::filesystem::path p(hpx::util::create_path(file));
            exception_ = detail::get_exception(hpx::exception(e, "", mode),
                func, p.string(), line);
        }
    }

    error_code::error_code(error e, char const* msg, throwmode mode)
      : boost::system::error_code(make_system_error_code(e, mode))
    {
        if (e != success && e != no_success && !(mode & lightweight))
            exception_ = detail::get_exception(hpx::exception(e, msg, mode));
    }

    error_code::error_code(error e, char const* msg,
            char const* func, char const* file, long line, throwmode mode)
      : boost::system::error_code(make_system_error_code(e, mode))
    {
        if (e != success && e != no_success && !(mode & lightweight)) {
            boost::filesystem::path p(hpx::util::create_path(file));
            exception_ = detail::get_exception(hpx::exception(e, msg, mode),
                func, p.string(), line);
        }
    }

    error_code::error_code(error e, std::string const& msg,
            throwmode mode)
      : boost::system::error_code(make_system_error_code(e, mode))
    {
        if (e != success && e != no_success && !(mode & lightweight))
            exception_ = detail::get_exception(hpx::exception(e, msg, mode));
    }

    error_code::error_code(error e, std::string const& msg,
            char const* func, char const* file, long line, throwmode mode)
      : boost::system::error_code(make_system_error_code(e, mode))
    {
        if (e != success && e != no_success && !(mode & lightweight)) {
            boost::filesystem::path p(hpx::util::create_path(file));
            exception_ = detail::get_exception(hpx::exception(e, msg, mode),
                func, p.string(), line);
        }
    }

    boost::exception_ptr get_exception_ptr(hpx::exception const& e)
    {
        try {
            throw e;
        }
        catch (...) {
            return boost::current_exception();
        }
    }

    error_code::error_code(int err, hpx::exception const& e)
    {
        this->boost::system::error_code::assign(err, get_hpx_category());
        try {
            throw e;
        }
        catch (...) {
            exception_ = get_exception_ptr(e);
        }
    }

    error_code::error_code(boost::exception_ptr const& e)
      : boost::system::error_code(make_system_error_code(get_error(e), rethrow)),
        exception_(e)
    {}

    ///////////////////////////////////////////////////////////////////////////
    std::string error_code::get_message() const
    {
        if (exception_) {
            try {
                boost::rethrow_exception(exception_);
            }
            catch (boost::exception const& be) {
                return dynamic_cast<std::exception const*>(&be)->what();
            }
        }
        return get_error_what(*this);   // provide at least minimal error text
    }

    ///////////////////////////////////////////////////////////////////////////
    error_code& error_code::operator=(error_code const& rhs)
    {
        if (this != &rhs) {
            if (rhs.value() == success) {
                // if the rhs is a success code, we maintain our throw mode
                this->boost::system::error_code::operator=(
                    make_success_code(
                        (category() == get_lightweight_hpx_category()) ?
                            hpx::lightweight : hpx::plain));
            }
            else {
                this->boost::system::error_code::operator=(rhs);
            }
            exception_ = rhs.exception_;
        }
        return *this;
    }

    void error_code::clear()
    {
        this->boost::system::error_code::assign(success, get_hpx_category());
        exception_ = boost::exception_ptr();
    }
}
