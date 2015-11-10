//  Copyright (c) 2014-2015 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_PARCELSET_CCI_SENDER_CONNECTION_HPP
#define HPX_PARCELSET_CCI_SENDER_CONNECTION_HPP

namespace hpx { namespace parcelset { namespace policies { namespace cci
{
    class HPX_EXPORT parcelport;

    struct sender_connection
      : parcelset::parcelport_connection<
            sender_connection
          , std::vector<char>
        >
    {
        sender_connection(cci_endpoint_t* endpoint, parcelset::locality there)
          : there_(std::move(there))
          , connection_(0)
        {
            std::string const& server_uri = there_.get<locality>().uri();
            CCI_ENSURE(
                cci_connect(
                    endpoint
                  , server_uri.c_str()
                  , NULL, 0
                  , CCI_CONN_ATTR_RU
                  , this, 0, NULL
                )
              , endpoint
            );
        }

        ~sender_connection()
        {
            message m(message::disconnect());
            cci_send(connection_, &m, sizeof(message), NULL, CCI_FLAG_SILENT);
            cci_disconnect(connection_.load());
        }

        parcelset::locality const& destination() const
        {
            return there_;
        }

        void verify(parcelset::locality const & parcel_locality_id) const
        {
        }

        template <typename Handler, typename ParcelPostprocess>
        void async_write(Handler && handler, ParcelPostprocess && parcel_postprocess)
        {
            handler_ = std::forward<Handler>(handler);
            postprocess_handler_ = std::forward<ParcelPostprocess>(parcel_postprocess);
            header_.reset(buffer_, connection_.load()->endpoint);

            // Saving a reference to ourselves to keep us alive
            // This will get released after we called the postprocess handler...
            this_ = shared_from_this();

            message m(message::rma_handle(header_.this_handle(), header_.data_.size()));
            CCI_ENSURE(
                cci_send(connection_, &m, sizeof(message), NULL, CCI_FLAG_SILENT)
              , connection_.load()->endpoint
            );
        }

        void done()
        {
            header_.deregister();
            {
                error_code ec;
                handler_(ec);
            }
            buffer_.clear();
            boost::shared_ptr<sender_connection> tmp(this_);
            this_.reset();
            {
                error_code ec;
                postprocess_handler_(ec, there_, tmp);
            }
        }

        cci_connection_t* connection()
        {
            std::size_t k = 0;
            cci_connection_t* conn = connection_.load();
            while(conn == 0)
            {
                hpx::lcos::local::spinlock::yield(k);
                conn = connection_.load();
                ++k;
            }
            return conn;
        }

        parcelset::locality there_;
        boost::atomic<cci_connection_t*> connection_;
        header header_;

        util::unique_function_nonser<
            void(
                error_code const&
            )
        > handler_;
        util::unique_function_nonser<
            void(
                error_code const&
              , parcelset::locality const&
              , boost::shared_ptr<sender_connection>
            )
        > postprocess_handler_;

        boost::shared_ptr<sender_connection> this_;
    };
}}}}

#endif
