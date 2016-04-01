//  Copyright (c) 2007-2016 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_LCOS_DETAIL_FUTURE_DATA_HPP
#define HPX_LCOS_DETAIL_FUTURE_DATA_HPP

#include <hpx/config.hpp>
#include <hpx/error_code.hpp>
#include <hpx/throw_exception.hpp>
#include <hpx/lcos/local/detail/condition_variable.hpp>
#include <hpx/lcos/local/spinlock.hpp>
#include <hpx/traits/get_remote_result.hpp>
#include <hpx/util/assert.hpp>
#include <hpx/util/atomic_count.hpp>
#include <hpx/util/deferred_call.hpp>
#include <hpx/util/unique_function.hpp>
#include <hpx/util/unused.hpp>

#include <boost/chrono/chrono.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/intrusive_ptr.hpp>

#include <cstddef>
#include <mutex>
#include <type_traits>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace lcos
{
    enum class future_status
    {
        ready, timeout, deferred, uninitialized
    };
}}

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace lcos { namespace detail
{
    template <typename Result> struct future_data;

    ///////////////////////////////////////////////////////////////////////
    struct future_data_refcnt_base;

    void intrusive_ptr_add_ref(future_data_refcnt_base* p);
    void intrusive_ptr_release(future_data_refcnt_base* p);

    ///////////////////////////////////////////////////////////////////////
    struct future_data_refcnt_base
    {
    private:
        typedef util::unique_function_nonser<void()> completed_callback_type;

    public:
        virtual ~future_data_refcnt_base() {}

        virtual void set_on_completed(completed_callback_type) = 0;

        virtual bool requires_delete()
        {
            return 0 == --count_;
        }

        virtual void destroy()
        {
            delete this;
        }

    protected:
        future_data_refcnt_base() : count_(0) {}

        // reference counting
        friend void intrusive_ptr_add_ref(future_data_refcnt_base* p);
        friend void intrusive_ptr_release(future_data_refcnt_base* p);

        util::atomic_count count_;
    };

    /// support functions for boost::intrusive_ptr
    inline void intrusive_ptr_add_ref(future_data_refcnt_base* p)
    {
        ++p->count_;
    }
    inline void intrusive_ptr_release(future_data_refcnt_base* p)
    {
        if (p->requires_delete())
            p->destroy();
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Result>
    struct future_data_result
    {
        typedef Result type;

        template <typename U>
        HPX_FORCEINLINE static
        U && set(U && u)
        {
            return std::forward<U>(u);
        }
    };

    template <typename Result>
    struct future_data_result<Result&>
    {
        typedef Result* type;

        HPX_FORCEINLINE static
        Result* set(Result* u)
        {
            return u;
        }

        HPX_FORCEINLINE static
        Result* set(Result& u)
        {
            return &u;
        }
    };

    template <>
    struct future_data_result<void>
    {
        typedef util::unused_type type;

        HPX_FORCEINLINE static
        util::unused_type set(util::unused_type u)
        {
            return u;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <std::size_t ...Vs>
    struct static_max;

    template <std::size_t V0>
    struct static_max<V0>
      : std::integral_constant<std::size_t, V0>
    {};

    template <std::size_t V0, std::size_t V1, std::size_t ...Vs>
    struct static_max<V0, V1, Vs...>
      : static_max<V0 < V1 ? V1 : V0, Vs...>
    {};

    template <std::size_t Len, typename ...Types>
    struct aligned_union
      : std::aligned_storage<
            static_max<Len, sizeof(Types)...>::value
          , static_max<std::alignment_of<Types>::value...>::value
        >
    {};

    template <typename R>
    struct future_data_storage
    {
        typedef typename future_data_result<R>::type value_type;
        typedef boost::exception_ptr error_type;

        typedef typename aligned_union<0u, value_type, error_type>::type type;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename F1, typename F2>
    class compose_cb_impl
    {
        HPX_MOVABLE_ONLY(compose_cb_impl);

    public:
        template <typename A1, typename A2>
        compose_cb_impl(A1 && f1, A2 && f2)
          : f1_(std::forward<A1>(f1))
          , f2_(std::forward<A2>(f2))
        {}

        compose_cb_impl(compose_cb_impl&& other)
          : f1_(std::move(other.f1_))
          , f2_(std::move(other.f2_))
        {}

        void operator()() const
        {
            f1_();
            f2_();
        }

    private:
        F1 f1_;
        F2 f2_;
    };

    template <typename F1, typename F2>
    static HPX_FORCEINLINE util::unique_function_nonser<void()>
    compose_cb(F1 && f1, F2 && f2)
    {
        if (!f1)
            return std::forward<F2>(f2);
        else if (!f2)
            return std::forward<F1>(f1);

        // otherwise create a combined callback
        typedef compose_cb_impl<
            typename std::decay<F1>::type, typename std::decay<F2>::type
        > result_type;
        return result_type(std::forward<F1>(f1), std::forward<F2>(f2));
    }

    ///////////////////////////////////////////////////////////////////////////
    struct handle_continuation_recursion_count
    {
        handle_continuation_recursion_count()
          : count_(threads::get_continuation_recursion_count())
        {
            ++count_;
        }
        ~handle_continuation_recursion_count()
        {
            --count_;
        }

        std::size_t& count_;
    };

    ///////////////////////////////////////////////////////////////////////////
    HPX_EXPORT bool run_on_completed_on_new_thread(
        util::unique_function_nonser<bool()> && f, error_code& ec);

    ///////////////////////////////////////////////////////////////////////////
    template <typename Result>
    struct future_data : future_data_refcnt_base
    {
        HPX_NON_COPYABLE(future_data);

        typedef typename future_data_result<Result>::type result_type;
        typedef util::unique_function_nonser<void()> completed_callback_type;
        typedef lcos::local::spinlock mutex_type;

        enum state
        {
            empty = 0,
            ready = 1,
            value = 2 | ready,
            exception = 4 | ready
        };

    public:
        future_data()
          : state_(empty)
        {}

        ~future_data()
        {
            reset();
        }

        virtual void execute_deferred(error_code& ec = throws) {}

        // cancellation is disabled by default
        virtual bool cancelable() const
        {
            return false;
        }
        virtual void cancel()
        {
            HPX_THROW_EXCEPTION(future_does_not_support_cancellation,
                "future_data::cancel",
                "this future does not support cancellation");
        }

        /// Get the result of the requested action. This call blocks (yields
        /// control) if the result is not ready. As soon as the result has been
        /// returned and the waiting thread has been re-scheduled by the thread
        /// manager the function will return.
        ///
        /// \param ec     [in,out] this represents the error status on exit,
        ///               if this is pre-initialized to \a hpx#throws
        ///               the function will throw on error instead. If the
        ///               operation blocks and is aborted because the object
        ///               went out of scope, the code \a hpx#yield_aborted is
        ///               set or thrown.
        ///
        /// \note         If there has been an error reported (using the action
        ///               \a base_lco#set_exception), this function will throw an
        ///               exception encapsulating the reported error code and
        ///               error description if <code>&ec == &throws</code>.
        virtual result_type* get_result(error_code& ec = throws)
        {
            // yields control if needed
            wait(ec);
            if (ec) return NULL;

            // No locking is required. Once a future has been made ready, which
            // is a postcondition of wait, either:
            //
            // - there is only one writer (future), or
            // - there are multiple readers only (shared_future, lock hurts
            //   concurrency)

            if (state_ == empty) {
                // the value has already been moved out of this future
                HPX_THROWS_IF(ec, no_state,
                    "future_data::get_result",
                    "this future has no valid shared state");
                return NULL;
            }

            // the thread has been re-activated by one of the actions
            // supported by this promise (see promise::set_event
            // and promise::set_exception).
            if (state_ == exception)
            {
                boost::exception_ptr* exception_ptr =
                    reinterpret_cast<boost::exception_ptr*>(&storage_);
                // an error has been reported in the meantime, throw or set
                // the error code
                if (&ec == &throws) {
                    boost::rethrow_exception(*exception_ptr);
                    // never reached
                }
                else {
                    ec = make_error_code(*exception_ptr);
                }
                return NULL;
            }
            return reinterpret_cast<result_type*>(&storage_);
        }

        // deferred execution of a given continuation
        bool run_on_completed(completed_callback_type && on_completed,
            boost::exception_ptr& ptr)
        {
            try {
                on_completed();
            }
            catch (...) {
                ptr = boost::current_exception();
                return false;
            }
            return true;
        }

        // make sure continuation invocation does not recurse deeper than
        // allowed
        void handle_on_completed(completed_callback_type && on_completed)
        {
#if defined(HPX_WINDOWS)
            bool recurse_asynchronously = false;
#elif defined(HPX_HAVE_THREADS_GET_STACK_POINTER)
            std::ptrdiff_t remaining_stack =
                this_thread::get_available_stack_space();

            if(remaining_stack < 0)
            {
                HPX_THROW_EXCEPTION(out_of_memory,
                    "future_data::handle_on_completed",
                    "Stack overflow");
            }
            bool recurse_asynchronously =
                remaining_stack < 8 * HPX_THREADS_STACK_OVERHEAD;
#else
            handle_continuation_recursion_count cnt;
            bool recurse_asynchronously =
                cnt.count_ > HPX_CONTINUATION_MAX_RECURSION_DEPTH;
#endif

            if (!recurse_asynchronously)
            {
                // directly execute continuation on this thread
                on_completed();
            }
            else
            {
                // re-spawn continuation on a new thread
                boost::intrusive_ptr<future_data> this_(this);

                error_code ec(lightweight);
                boost::exception_ptr ptr;
                if (!run_on_completed_on_new_thread(
                        util::deferred_call(&future_data::run_on_completed,
                            std::move(this_), std::move(on_completed),
                            boost::ref(ptr)),
                        ec))
                {
                    // thread creation went wrong
                    if (ec) {
                        set_exception(hpx::detail::access_exception(ec));
                        return;
                    }

                    // re-throw exception in this context
                    HPX_ASSERT(ptr);        // exception should have been set
                    boost::rethrow_exception(ptr);
                }
            }
        }

        /// Set the result of the requested action.
        template <typename Target>
        void set_value(Target && data, error_code& ec = throws)
        {
            std::unique_lock<mutex_type> l(this->mtx_);

            // check whether the data has already been set
            if (is_ready_locked()) {
                l.unlock();
                HPX_THROWS_IF(ec, promise_already_satisfied,
                    "future_data::set_value",
                    "data has already been set for this future");
                return;
            }

            completed_callback_type on_completed;

            on_completed = std::move(this->on_completed_);

            // set the data
            result_type* value_ptr =
                reinterpret_cast<result_type*>(&storage_);
            ::new ((void*)value_ptr) result_type(
                future_data_result<Result>::set(std::forward<Target>(data)));
            state_ = value;

            // handle all threads waiting for the future to become ready
            cond_.notify_all(std::move(l), ec);

            // Note: cv.notify_all() above 'consumes' the lock 'l' and leaves
            //       it unlocked when returning.

            // invoke the callback (continuation) function
            if (on_completed)
                handle_on_completed(std::move(on_completed));
        }

        template <typename Target>
        void set_exception(Target && data, error_code& ec = throws)
        {
            std::unique_lock<mutex_type> l(this->mtx_);

            // check whether the data has already been set
            if (is_ready_locked()) {
                l.unlock();
                HPX_THROWS_IF(ec, promise_already_satisfied,
                    "future_data::set_exception",
                    "data has already been set for this future");
                return;
            }

            completed_callback_type on_completed;

            on_completed = std::move(this->on_completed_);

            // set the data
            boost::exception_ptr* exception_ptr =
                reinterpret_cast<boost::exception_ptr*>(&storage_);
            ::new ((void*)exception_ptr) boost::exception_ptr(
                std::forward<Target>(data));
            state_ = exception;

            // handle all threads waiting for the future to become ready
            cond_.notify_all(std::move(l), ec);

            // Note: cv.notify_all() above 'consumes' the lock 'l' and leaves
            //       it unlocked when returning.

            // invoke the callback (continuation) function
            if (on_completed)
                handle_on_completed(std::move(on_completed));
        }

        // helper functions for setting data (if successful) or the error (if
        // non-successful)
        template <typename T>
        void set_data(T && result)
        {
            // set the received result, reset error status
            try {
                typedef typename std::decay<T>::type naked_type;

                typedef traits::get_remote_result<
                    result_type, naked_type
                > get_remote_result_type;

                // store the value
                set_value(std::move(get_remote_result_type::call(
                        std::forward<T>(result))));
            }
            catch (...) {
                // store the error instead
                return set_exception(boost::current_exception());
            }
        }

        // trigger the future with the given error condition
        void set_error(error e, char const* f, char const* msg)
        {
            try {
                HPX_THROW_EXCEPTION(e, f, msg);
            }
            catch (...) {
                // store the error code
                set_exception(boost::current_exception());
            }
        }

        /// Reset the promise to allow to restart an asynchronous
        /// operation. Allows any subsequent set_data operation to succeed.
        void reset(error_code& /*ec*/ = throws)
        {
            // no locking is required as semantics guarantee a single writer
            // and no reader

            // release any stored data and callback functions
            switch (state_) {
            case value:
            {
                result_type* value_ptr =
                    reinterpret_cast<result_type*>(&storage_);
                value_ptr->~result_type();
                break;
            }
            case exception:
            {
                boost::exception_ptr* exception_ptr =
                    reinterpret_cast<boost::exception_ptr*>(&storage_);
                exception_ptr->~exception_ptr();
                break;
            }
            default: break;
            }

            state_ = empty;
            on_completed_ = completed_callback_type();
        }

        // continuation support

        /// Set the callback which needs to be invoked when the future becomes
        /// ready. If the future is ready the function will be invoked
        /// immediately.
        void set_on_completed(completed_callback_type data_sink)
        {
            if (!data_sink) return;

            std::unique_lock<mutex_type> l(this->mtx_);

            if (is_ready_locked()) {

                HPX_ASSERT(!on_completed_);

                // invoke the callback (continuation) function right away
                l.unlock();

                handle_on_completed(std::move(data_sink));
            }
            else {
                // store a combined callback wrapping the old and the new one
                this->on_completed_ = compose_cb(
                    std::move(data_sink), std::move(on_completed_));
            }
        }

        virtual void wait(error_code& ec = throws)
        {
            std::unique_lock<mutex_type> l(mtx_);

            // block if this entry is empty
            if (state_ == empty) {
                cond_.wait(std::move(l), "future_data::wait", ec);
                if (ec) return;
            }

            if (&ec != &throws)
                ec = make_success_code();
        }

        virtual future_status
        wait_until(boost::chrono::steady_clock::time_point const& abs_time,
            error_code& ec = throws)
        {
            std::unique_lock<mutex_type> l(mtx_);

            // block if this entry is empty
            if (state_ == empty) {
                threads::thread_state_ex_enum const reason =
                    cond_.wait_until(std::move(l), abs_time,
                        "future_data::wait_until", ec);
                if (ec) return future_status::uninitialized;

                if (reason == threads::wait_timeout)
                    return future_status::timeout;

                return future_status::ready;
            }

            if (&ec != &throws)
                ec = make_success_code();

            return future_status::ready; //-V110
        }

        /// Return whether or not the data is available for this
        /// \a future.
        bool is_ready() const
        {
            std::unique_lock<mutex_type> l(mtx_);
            return is_ready_locked();
        }

        bool is_ready_locked() const
        {
            return (state_ & ready) != 0;
        }

        bool has_value() const
        {
            std::unique_lock<mutex_type> l(mtx_);
            return state_ == value;
        }

        bool has_exception() const
        {
            std::unique_lock<mutex_type> l(mtx_);
            return state_ == exception;
        }

    protected:
        mutable mutex_type mtx_;
        completed_callback_type on_completed_;

    private:
        local::detail::condition_variable cond_;    // threads waiting in read
        state state_;                               // current state
        typename future_data_storage<Result>::type storage_;
    };
}}}

#endif /*HPX_LCOS_DETAIL_FUTURE_DATA_HPP*/
