//  Copyright (c) 2007-2016 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_LCOS_DETAIL_TIMED_FUTURE_DATA_HPP
#define HPX_LCOS_DETAIL_TIMED_FUTURE_DATA_HPP

#include <hpx/config.hpp>
#include <hpx/error_code.hpp>
#include <hpx/lcos/detail/future_data.hpp>
#include <hpx/runtime/threads/thread_helpers.hpp>
#include <hpx/util/deferred_call.hpp>

#include <boost/chrono/chrono.hpp>
#include <boost/intrusive_ptr.hpp>

#include <utility>

namespace hpx { namespace lcos { namespace detail
{
    template <typename Result>
    struct timed_future_data : future_data<Result>
    {
    public:
        typedef future_data<Result> base_type;
        typedef typename base_type::result_type result_type;
        typedef typename base_type::mutex_type mutex_type;

    public:
        timed_future_data() {}

        template <typename Result_>
        timed_future_data(
            boost::chrono::steady_clock::time_point const& abs_time,
            Result_&& init)
        {
            boost::intrusive_ptr<timed_future_data> this_(this);

            error_code ec;
            threads::thread_id_type id = threads::register_thread_nullary(
                util::deferred_call(&timed_future_data::set_value,
                    std::move(this_),
                    future_data_result<Result>::set(std::forward<Result_>(init))),
                "timed_future_data<Result>::timed_future_data",
                threads::suspended, true, threads::thread_priority_normal,
                std::size_t(-1), threads::thread_stacksize_default, ec);
            if (ec) {
                // thread creation failed, report error to the new future
                this->base_type::set_exception(hpx::detail::access_exception(ec));
            }

            // start new thread at given point in time
            threads::set_thread_state(id, abs_time, threads::pending,
                threads::wait_timeout, threads::thread_priority_boost, ec);
            if (ec) {
                // thread scheduling failed, report error to the new future
                this->base_type::set_exception(hpx::detail::access_exception(ec));
            }
        }

        void set_value(result_type const& value)
        {
            this->base_type::set_value(value);
        }
    };
}}}

#endif /*HPX_LCOS_DETAIL_TIMED_FUTURE_DATA_HPP*/
