//  Copyright (c) 2007-2015 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//  Copyright (c) 2008-2009 Chirag Dekate, Anshul Tandon
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_RUNTIME_THREADS_THIS_THREAD_HPP
#define HPX_RUNTIME_THREADS_THIS_THREAD_HPP

#include <hpx/config.hpp>
#include <hpx/exception_fwd.hpp>
#include <hpx/runtime/threads/thread_enums.hpp>
#include <hpx/runtime/threads/executors/current_executor.hpp>
#include <hpx/util/date_time_chrono.hpp>

#include <boost/cstdint.hpp>

namespace hpx { namespace this_thread {
    ///////////////////////////////////////////////////////////////////////////
    /// The function \a suspend will return control to the thread manager
    /// (suspends the current thread). It sets the new state of this thread
    /// to the thread state passed as the parameter.
    ///
    /// \note Must be called from within a HPX-thread.
    ///
    /// \throws If <code>&ec != &throws</code>, never throws, but will set \a ec
    ///         to an appropriate value when an error occurs. Otherwise, this
    ///         function will throw an \a hpx#exception with an error code of
    ///         \a hpx#yield_aborted if it is signaled with \a wait_aborted.
    ///         If called outside of a HPX-thread, this function will throw
    ///         an \a hpx#exception with an error code of \a hpx::null_thread_id.
    ///         If this function is called while the thread-manager is not
    ///         running, it will throw an \a hpx#exception with an error code of
    ///         \a hpx#invalid_status.
    ///
    HPX_API_EXPORT threads::thread_state_ex_enum suspend(
        threads::thread_state_enum state = threads::pending,
        char const* description = "this_thread::suspend",
        error_code& ec = throws);

    /// The function \a suspend will return control to the thread manager
    /// (suspends the current thread). It sets the new state of this thread
    /// to \a suspended and schedules a wakeup for this threads at the given
    /// time.
    ///
    /// \note Must be called from within a HPX-thread.
    ///
    /// \throws If <code>&ec != &throws</code>, never throws, but will set \a ec
    ///         to an appropriate value when an error occurs. Otherwise, this
    ///         function will throw an \a hpx#exception with an error code of
    ///         \a hpx#yield_aborted if it is signaled with \a wait_aborted.
    ///         If called outside of a HPX-thread, this function will throw
    ///         an \a hpx#exception with an error code of \a hpx::null_thread_id.
    ///         If this function is called while the thread-manager is not
    ///         running, it will throw an \a hpx#exception with an error code of
    ///         \a hpx#invalid_status.
    ///
    HPX_API_EXPORT threads::thread_state_ex_enum suspend(
        util::steady_time_point const& abs_time,
        char const* description = "this_thread::suspend",
        error_code& ec = throws);

    /// The function \a suspend will return control to the thread manager
    /// (suspends the current thread). It sets the new state of this thread
    /// to \a suspended and schedules a wakeup for this threads after the given
    /// duration.
    ///
    /// \note Must be called from within a HPX-thread.
    ///
    /// \throws If <code>&ec != &throws</code>, never throws, but will set \a ec
    ///         to an appropriate value when an error occurs. Otherwise, this
    ///         function will throw an \a hpx#exception with an error code of
    ///         \a hpx#yield_aborted if it is signaled with \a wait_aborted.
    ///         If called outside of a HPX-thread, this function will throw
    ///         an \a hpx#exception with an error code of \a hpx::null_thread_id.
    ///         If this function is called while the thread-manager is not
    ///         running, it will throw an \a hpx#exception with an error code of
    ///         \a hpx#invalid_status.
    ///
    inline threads::thread_state_ex_enum suspend(
        util::steady_duration const& rel_time,
        char const* description = "this_thread::suspend",
        error_code& ec = throws)
    {
        return suspend(rel_time.from_now(), description, ec);
    }

    /// The function \a suspend will return control to the thread manager
    /// (suspends the current thread). It sets the new state of this thread
    /// to \a suspended and schedules a wakeup for this threads after the given
    /// time (specified in milliseconds).
    ///
    /// \note Must be called from within a HPX-thread.
    ///
    /// \throws If <code>&ec != &throws</code>, never throws, but will set \a ec
    ///         to an appropriate value when an error occurs. Otherwise, this
    ///         function will throw an \a hpx#exception with an error code of
    ///         \a hpx#yield_aborted if it is signaled with \a wait_aborted.
    ///         If called outside of a HPX-thread, this function will throw
    ///         an \a hpx#exception with an error code of \a hpx::null_thread_id.
    ///         If this function is called while the thread-manager is not
    ///         running, it will throw an \a hpx#exception with an error code of
    ///         \a hpx#invalid_status.
    ///
    inline threads::thread_state_ex_enum suspend(
        boost::uint64_t ms, char const* description = "this_thread::suspend",
        error_code& ec = throws)
    {
        return suspend(boost::chrono::milliseconds(ms), description, ec);
    }

    /// Returns a reference to the executor which was used to create the current
    /// thread.
    ///
    /// \throws If <code>&ec != &throws</code>, never throws, but will set \a ec
    ///         to an appropriate value when an error occurs. Otherwise, this
    ///         function will throw an \a hpx#exception with an error code of
    ///         \a hpx#yield_aborted if it is signaled with \a wait_aborted.
    ///         If called outside of a HPX-thread, this function will throw
    ///         an \a hpx#exception with an error code of \a hpx::null_thread_id.
    ///         If this function is called while the thread-manager is not
    ///         running, it will throw an \a hpx#exception with an error code of
    ///         \a hpx#invalid_status.
    ///
    HPX_EXPORT threads::executors::current_executor
        get_executor(error_code& ec = throws);

    HPX_API_EXPORT void yield() HPX_NOEXCEPT;

    // extensions
    HPX_API_EXPORT threads::thread_priority get_priority();
    HPX_API_EXPORT std::ptrdiff_t get_stack_size();

    HPX_API_EXPORT void interruption_point();
    HPX_API_EXPORT bool interruption_enabled();
    HPX_API_EXPORT bool interruption_requested();

    HPX_API_EXPORT void interrupt();

    HPX_API_EXPORT void sleep_until(util::steady_time_point const& abs_time);

    inline void sleep_for(util::steady_duration const& rel_time)
    {
        sleep_until(rel_time.from_now());
    }

    HPX_API_EXPORT std::size_t get_thread_data();
    HPX_API_EXPORT std::size_t set_thread_data(std::size_t);

    class HPX_EXPORT disable_interruption
    {
    private:
        disable_interruption(disable_interruption const&);
        disable_interruption& operator=(disable_interruption const&);

        bool interruption_was_enabled_;
        friend class restore_interruption;

    public:
        disable_interruption();
        ~disable_interruption();
    };

    class HPX_EXPORT restore_interruption
    {
    private:
        restore_interruption(restore_interruption const&);
        restore_interruption& operator=(restore_interruption const&);

        bool interruption_was_enabled_;

    public:
        explicit restore_interruption(disable_interruption& d);
        ~restore_interruption();
    };
}}

#endif
