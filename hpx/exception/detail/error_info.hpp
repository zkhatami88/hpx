//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_EXCEPTION_DETAIL_ERROR_INFO_HPP)
#define HPX_EXCEPTION_DETAIL_ERROR_INFO_HPP

#include <hpx/config.hpp>

#include <boost/cstdint.hpp>
#include <boost/exception/error_info.hpp>
#include <boost/exception/exception.hpp>

#include <string>

namespace hpx { namespace detail {
    ///////////////////////////////////////////////////////////////////////
    // types needed to add additional information to the thrown exceptions
    struct tag_throw_locality {};
    struct tag_throw_hostname {};
    struct tag_throw_pid {};
    struct tag_throw_shepherd {};
    struct tag_throw_thread_id {};
    struct tag_throw_thread_name {};
    struct tag_throw_file {};
    struct tag_throw_function {};
    struct tag_throw_stacktrace {};
    struct tag_throw_env {};
    struct tag_throw_config {};
    struct tag_throw_state {};
    struct tag_throw_auxinfo {};

    // Stores the information about the locality id the exception has been
    // raised on. This information will show up in error messages under the
    // [locality] tag.
    typedef boost::error_info<detail::tag_throw_locality, boost::uint32_t>
        throw_locality;

    // Stores the information about the hostname of the locality the exception
    // has been raised on. This information will show up in error messages
    // under the [hostname] tag.
    typedef boost::error_info<detail::tag_throw_hostname, std::string>
        throw_hostname;

    // Stores the information about the pid of the OS process the exception
    // has been raised on. This information will show up in error messages
    // under the [pid] tag.
    typedef boost::error_info<detail::tag_throw_pid, boost::int64_t>
        throw_pid;

    // Stores the information about the shepherd thread the exception has been
    // raised on. This information will show up in error messages under the
    // [shepherd] tag.
    typedef boost::error_info<detail::tag_throw_shepherd, std::size_t>
        throw_shepherd;

    // Stores the information about the HPX thread the exception has been
    // raised on. This information will show up in error messages under the
    // [thread_id] tag.
    typedef boost::error_info<detail::tag_throw_thread_id, std::size_t>
        throw_thread_id;

    // Stores the information about the HPX thread name the exception has been
    // raised on. This information will show up in error messages under the
    // [thread_name] tag.
    typedef boost::error_info<detail::tag_throw_thread_name, std::string>
        throw_thread_name;

    // Stores the information about the function name the exception has been
    // raised in. This information will show up in error messages under the
    // [function] tag.
    typedef boost::error_info<detail::tag_throw_function, std::string>
        throw_function;

    // Stores the information about the source file name the exception has
    // been raised in. This information will show up in error messages under
    // the [file] tag.
    typedef boost::error_info<detail::tag_throw_file, std::string>
        throw_file;

    // Stores the information about the source file line number the exception
    // has been raised at. This information will show up in error messages
    // under the [line] tag.
    using boost::throw_line;

    // Stores the information about the stack backtrace at the point the
    // exception has been raised at. This information will show up in error
    // messages under the [stack_trace] tag.
    typedef boost::error_info<detail::tag_throw_stacktrace, std::string>
        throw_stacktrace;

    // Stores the full execution environment of the locality the exception
    // has been raised in. This information will show up in error messages
    // under the [env] tag.
    typedef boost::error_info<detail::tag_throw_env, std::string>
        throw_env;

    // Stores the full HPX configuration information of the locality the
    // exception has been raised in. This information will show up in error
    // messages under the [config] tag.
    typedef boost::error_info<detail::tag_throw_config, std::string>
        throw_config;

    // Stores the current runtime state. This information will show up in
    // error messages under the [state] tag.
    typedef boost::error_info<detail::tag_throw_state, std::string>
        throw_state;

    // Stores additional auxiliary information (such as information about
    // the current parcel). This information will show up in error messages
    // under the [auxinfo] tag.
    typedef boost::error_info<detail::tag_throw_auxinfo, std::string>
        throw_auxinfo;
}}

#endif
