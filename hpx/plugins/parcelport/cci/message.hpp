//  Copyright (c) 2014-2015 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_PARCELSET_CCI_MESSAGE_HPP
#define HPX_PARCELSET_CCI_MESSAGE_HPP

#include <cci.h>

namespace hpx { namespace parcelset {  namespace policies { namespace cci
{
    struct message
    {
        enum message_type {
            send_rma_handle,
            send_done,
            send_disconnect,
            send_invalid
        };

        static message done()
        {
            return message(send_done);
        }

        static message disconnect()
        {
            return message(send_disconnect);
        }

        static message rma_handle(cci_rma_handle_t const & handle, std::size_t size)
        {
            message ret(send_rma_handle);
            ret.size_ = size;
            ret.rma_handle_ = handle;

            return ret;
        }

        message()
          : type_(send_invalid)
        {}

        message(message_type type)
          : type_(type)
        {}

        std::size_t size_;
        cci_rma_handle rma_handle_;
        message_type type_;
    };
}}}}

#endif
