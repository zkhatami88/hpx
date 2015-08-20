//  Copyright (c) 2013-2014 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_SERIALIZATION_SERIALIZE_BUFFER_FWD_HPP)
#define HPX_SERIALIZATION_SERIALIZE_BUFFER_FWD_HPP

#include <boost/shared_array.hpp>

namespace hpx { namespace serialization {
    namespace detail
    {
        struct serialize_buffer_no_allocator {};
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename Allocator = detail::serialize_buffer_no_allocator>
    class serialize_buffer;

    namespace detail
    {
        template <typename T, typename Allocator>
        boost::shared_array<T> & get_shared_buffer_data(serialize_buffer<T, Allocator> & buf);
    }
}}

#endif
