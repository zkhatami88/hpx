//  Copyright (c) 2007-2009 Hartmut Kaiser
//
//  Parts of this code were taken from the Boost.Asio library
//  Copyright (c) 2003-2007 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <stdexcept>

#include <hpx/hpx_fwd.hpp>
#include <hpx/util/io_service_pool.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace util
{
    io_service_pool::io_service_pool(std::size_t pool_size, 
            boost::function<void()> on_start_thread,
            boost::function<void()> on_stop_thread)
      : next_io_service_(0), stopped_(false), 
        on_start_thread_(on_start_thread), on_stop_thread_(on_stop_thread)
    {
        if (pool_size == 0)
            throw std::runtime_error("io_service_pool size is 0");

        // Give all the io_services work to do so that their run() functions 
        // will not exit until they are explicitly stopped.
        for (std::size_t i = 0; i < pool_size; ++i)
        {
            io_service_ptr io_service(new boost::asio::io_service);
            work_ptr work(new boost::asio::io_service::work(*io_service));
            io_services_.push_back(io_service);
            work_.push_back(work);
        }
    }

    io_service_pool::io_service_pool(boost::function<void()> on_start_thread,
            boost::function<void()> on_stop_thread)
      : next_io_service_(0), stopped_(false), 
        on_start_thread_(on_start_thread), on_stop_thread_(on_stop_thread)
    {
        io_service_ptr io_service(new boost::asio::io_service);
        work_ptr work(new boost::asio::io_service::work(*io_service));
        io_services_.push_back(io_service);
        work_.push_back(work);
    }

    void io_service_pool::thread_run(int index)
    {
        if (on_start_thread_)
            on_start_thread_();

//         boost::system::error_code ec;
//         do {
//             // use this thread for the given io service
//             io_services_[index]->run(ec);   // run io service
//             if (!ec)
//                 break;
// 
//             // reset io service on error and restart the thread loop
//             io_services_[index]->reset();
//         } while (true);

        // use this thread for the given io service
        io_services_[index]->run();   // run io service

        if (on_stop_thread_)
            on_stop_thread_();
    }

    bool io_service_pool::run(bool join_threads)
    {
        // Create a pool of threads to run all of the io_services.
        if (!threads_.empty())   // should be called only once
        {
            if (join_threads) 
                join();
            return false;
        }

        for (std::size_t i = 0; i < io_services_.size(); ++i)
        {
            boost::shared_ptr<boost::thread> thread(new boost::thread(
                boost::bind(&io_service_pool::thread_run, this, i)));
            threads_.push_back(thread);
        }
        if (join_threads)
            join();

        return true;
    }

    void io_service_pool::join()
    {
        // Wait for all threads in the pool to exit.
        for (std::size_t i = 0; /*!stopped_ && */i < threads_.size(); ++i)
            threads_[i]->join();
    }

    void io_service_pool::stop()
    {
        if (!stopped_) {
            // Explicitly inform all work to exit.
            for (std::size_t i = 0; i < work_.size(); ++i)
                work_[i].reset();

            // Explicitly stop all io_services.
            for (std::size_t i = 0; !stopped_ && i < io_services_.size(); ++i)
                io_services_[i]->stop();

            stopped_ = true;
        }
    }

    void io_service_pool::clear()
    {
        if (stopped_) {
            threads_.clear();
            io_services_.clear();
            work_.clear();
        }
    }

    boost::asio::io_service& io_service_pool::get_io_service()
    {
        // Use a round-robin scheme to choose the next io_service to use.
        boost::asio::io_service& io_service = *io_services_[next_io_service_];
        ++next_io_service_;
        if (next_io_service_ == io_services_.size())
            next_io_service_ = 0;
        return io_service;
    }

///////////////////////////////////////////////////////////////////////////////
}}  // namespace hpx::util

