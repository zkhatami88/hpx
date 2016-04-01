//  Copyright (c) 2007-2016 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_LCOS_DETAIL_TASK_FUTURE_DATA_HPP
#define HPX_LCOS_DETAIL_TASK_FUTURE_DATA_HPP

#include <hpx/config.hpp>
#include <hpx/error_code.hpp>
#include <hpx/throw_exception.hpp>
#include <hpx/lcos/detail/future_data.hpp>
#include <hpx/runtime/threads/thread_data_fwd.hpp>
#include <hpx/runtime/threads/thread_data_fwd.hpp>
#include <hpx/util/assert.hpp>

#include <boost/chrono/chrono.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/thread/locks.hpp>

#include <mutex>
#include <utility>

namespace hpx { namespace lcos { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename Result>
    struct task_future_data : future_data<Result>
    {
    protected:
        typedef typename future_data<Result>::mutex_type mutex_type;
        typedef boost::intrusive_ptr<task_future_data> future_base_type;
        typedef typename future_data<Result>::result_type result_type;

    public:
        task_future_data()
          : started_(false), sched_(0)
        {}

        task_future_data(threads::executor& sched)
          : started_(false),
            sched_(sched ? &sched : 0)
        {}

        virtual void execute_deferred(error_code& ec = throws)
        {
            if (!started_test_and_set())
                this->do_run();
        }

        // retrieving the value
        virtual result_type* get_result(error_code& ec = throws)
        {
            if (!started_test_and_set())
                this->do_run();
            return this->future_data<Result>::get_result(ec);
        }

        // wait support
        virtual void wait(error_code& ec = throws)
        {
            if (!started_test_and_set())
                this->do_run();
            this->future_data<Result>::wait(ec);
        }

        virtual future_status
        wait_until(boost::chrono::steady_clock::time_point const& abs_time,
            error_code& ec = throws)
        {
            if (!started_test())
                return future_status::deferred; //-V110
            return this->future_data<Result>::wait_until(abs_time, ec);
        };

    private:
        bool started_test() const
        {
            std::lock_guard<mutex_type> l(this->mtx_);
            return started_;
        }

        bool started_test_and_set()
        {
            std::lock_guard<mutex_type> l(this->mtx_);
            if (started_)
                return true;

            started_ = true;
            return false;
        }

    protected:
        void check_started()
        {
            std::lock_guard<mutex_type> l(this->mtx_);
            if (started_) {
                HPX_THROW_EXCEPTION(task_already_started,
                    "task_future_data::check_started",
                    "this task has already been started");
                return;
            }
            started_ = true;
        }

    public:
        // run synchronously
        void run()
        {
            check_started();
            this->do_run();       // always on this thread
        }

        // run in a separate thread
        virtual void apply(launch policy,
            threads::thread_priority priority,
            threads::thread_stacksize stacksize, error_code& ec)
        {
            HPX_ASSERT(false);      // shouldn't ever be called
        }

    protected:
        static threads::thread_state_enum run_impl(future_base_type this_)
        {
            this_->do_run();
            return threads::terminated;
        }

    public:
        template <typename T>
        void set_data(T && result)
        {
            this->future_data<Result>::set_data(std::forward<T>(result));
        }

        void set_exception(boost::exception_ptr const& e)
        {
            this->future_data<Result>::set_exception(e);
        }

        virtual void do_run()
        {
            HPX_ASSERT(false);      // shouldn't ever be called
        }

    protected:
        bool started_;
        threads::executor* sched_;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Result>
    struct cancellable_task_future_data : task_future_data<Result>
    {
    protected:
        typedef typename task_future_data<Result>::mutex_type mutex_type;
        typedef boost::intrusive_ptr<cancellable_task_future_data> future_base_type;
        typedef typename future_data<Result>::result_type result_type;

    protected:
        threads::thread_id_type get_thread_id() const
        {
            std::lock_guard<mutex_type> l(this->mtx_);
            return id_;
        }
        void set_thread_id(threads::thread_id_type id)
        {
            std::lock_guard<mutex_type> l(this->mtx_);
            id_ = id;
        }

    public:
        cancellable_task_future_data()
          : task_future_data<Result>(), id_(threads::invalid_thread_id)
        {}

        cancellable_task_future_data(threads::executor& sched)
          : task_future_data<Result>(sched), id_(threads::invalid_thread_id)
        {}

    private:
        struct reset_id
        {
            reset_id(cancellable_task_future_data& target)
              : target_(target)
            {
                target.set_thread_id(threads::get_self_id());
            }
            ~reset_id()
            {
                target_.set_thread_id(threads::invalid_thread_id);
            }
            cancellable_task_future_data& target_;
        };

    protected:
        static threads::thread_state_enum run_impl(future_base_type this_)
        {
            reset_id r(*this_);
            this_->do_run();
            return threads::terminated;
        }

    public:
        // cancellation support
        bool cancelable() const
        {
            return true;
        }

        void cancel()
        {
            std::unique_lock<mutex_type> l(this->mtx_);
            try {
                if (!this->started_)
                    HPX_THROW_THREAD_INTERRUPTED_EXCEPTION();

                if (this->is_ready_locked())
                    return;   // nothing we can do

                if (id_ != threads::invalid_thread_id) {
                    // interrupt the executing thread
                    threads::interrupt_thread(id_);

                    this->started_ = true;

                    l.unlock();
                    this->set_error(future_cancelled,
                        "task_future_data<Result>::cancel",
                        "future has been canceled");
                }
                else {
                    HPX_THROW_EXCEPTION(future_can_not_be_cancelled,
                        "task_future_data<Result>::cancel",
                        "future can't be canceled at this time");
                }
            }
            catch (...) {
                this->started_ = true;
                this->set_exception(boost::current_exception());
                throw;
            }
        }

    protected:
        threads::thread_id_type id_;
    };
}}}

#endif /*HPX_LCOS_DETAIL_TASK_FUTURE_DATA_HPP*/
