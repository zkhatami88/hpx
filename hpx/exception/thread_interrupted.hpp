//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_EXCEPTION_THREAD_INTERRUPTED_HPP)
#define HPX_EXCEPTION_THREAD_INTERRUPTED_HPP

#include <hpx/config.hpp>
#include <exception>

namespace hpx {
    ///////////////////////////////////////////////////////////////////////////
    /// \brief A hpx::thread_interrupted is the exception type used by HPX to
    ///        interrupt a running HPX thread.
    ///
    /// The \a hpx::thread_interrupted type is the exception type used by HPX to
    /// interrupt a running thread.
    ///
    /// A running thread can be interrupted by invoking the interrupt() member
    /// function of the corresponding hpx::thread object. When the interrupted
    /// thread next executes one of the specified interruption points (or if it
    /// is currently blocked whilst executing one) with interruption enabled,
    /// then a hpx::thread_interrupted exception will be thrown in the interrupted
    /// thread. If not caught, this will cause the execution of the interrupted
    /// thread to terminate. As with any other exception, the stack will be
    /// unwound, and destructors for objects of automatic storage duration will
    /// be executed.
    ///
    /// If a thread wishes to avoid being interrupted, it can create an instance
    /// of \a hpx::this_thread::disable_interruption. Objects of this class disable
    /// interruption for the thread that created them on construction, and
    /// restore the interruption state to whatever it was before on destruction.
    ///
    /// \code
    ///     void f()
    ///     {
    ///         // interruption enabled here
    ///         {
    ///             hpx::this_thread::disable_interruption di;
    ///             // interruption disabled
    ///             {
    ///                 hpx::this_thread::disable_interruption di2;
    ///                 // interruption still disabled
    ///             } // di2 destroyed, interruption state restored
    ///             // interruption still disabled
    ///         } // di destroyed, interruption state restored
    ///         // interruption now enabled
    ///     }
    /// \endcode
    ///
    /// The effects of an instance of \a hpx::this_thread::disable_interruption can be
    /// temporarily reversed by constructing an instance of
    /// \a hpx::this_thread::restore_interruption, passing in the
    /// \a hpx::this_thread::disable_interruption object in question. This will restore
    /// the interruption state to what it was when the
    /// \a hpx::this_thread::disable_interruption
    /// object was constructed, and then disable interruption again when the
    /// \a hpx::this_thread::restore_interruption object is destroyed.
    ///
    /// \code
    ///     void g()
    ///     {
    ///         // interruption enabled here
    ///         {
    ///             hpx::this_thread::disable_interruption di;
    ///             // interruption disabled
    ///             {
    ///                 hpx::this_thread::restore_interruption ri(di);
    ///                 // interruption now enabled
    ///             } // ri destroyed, interruption disable again
    ///         } // di destroyed, interruption state restored
    ///         // interruption now enabled
    ///     }
    /// \endcode
    ///
    /// At any point, the interruption state for the current thread can be
    /// queried by calling \a hpx::this_thread::interruption_enabled().
    struct HPX_EXCEPTION_EXPORT thread_interrupted : std::exception {};
}

#endif
