//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_EXCEPTION_DETAIL_STD_EXCEPTION_HPP)
#define HPX_EXCEPTION_DETAIL_STD_EXCEPTION_HPP

#include <hpx/config.hpp>

#include <exception> // exception, bad_exception
#include <new> // bad_alloc
#include <string>
#include <typeinfo> // bad_cast, bad_typeid

namespace hpx { namespace detail {
    struct HPX_EXCEPTION_EXPORT std_exception : std::exception
    {
      private:
        std::string what_;

      public:
        explicit std_exception(std::string const& w)
          : what_(w)
        {}

        ~std_exception() HPX_NOEXCEPT_OR_NOTHROW {}

        const char* what() const HPX_NOEXCEPT_OR_NOTHROW
        {
            return what_.c_str();
        }
    };

    struct HPX_EXCEPTION_EXPORT bad_alloc : std::bad_alloc
    {
      private:
        std::string what_;

      public:
        explicit bad_alloc(std::string const& w)
          : what_(w)
        {}

        ~bad_alloc() HPX_NOEXCEPT_OR_NOTHROW {}

        const char* what() const HPX_NOEXCEPT_OR_NOTHROW
        {
            return what_.c_str();
        }
    };

    struct HPX_EXCEPTION_EXPORT bad_exception : std::bad_exception
    {
      private:
        std::string what_;

      public:
        explicit bad_exception(std::string const& w)
          : what_(w)
        {}

        ~bad_exception() HPX_NOEXCEPT_OR_NOTHROW {}

        const char* what() const HPX_NOEXCEPT_OR_NOTHROW
        {
            return what_.c_str();
        }
    };

    struct HPX_EXCEPTION_EXPORT bad_cast : std::bad_cast
    {
      private:
        std::string what_;

      public:
        explicit bad_cast(std::string const& w)
          : what_(w)
        {}

        ~bad_cast() HPX_NOEXCEPT_OR_NOTHROW {}

        const char* what() const HPX_NOEXCEPT_OR_NOTHROW
        {
            return what_.c_str();
        }
    };

    struct HPX_EXCEPTION_EXPORT bad_typeid : std::bad_typeid
    {
      private:
        std::string what_;

      public:
        explicit bad_typeid(std::string const& w)
          : what_(w)
        {}

        ~bad_typeid() HPX_NOEXCEPT_OR_NOTHROW {}

        const char* what() const HPX_NOEXCEPT_OR_NOTHROW
        {
            return what_.c_str();
        }
    };
}}

#endif
