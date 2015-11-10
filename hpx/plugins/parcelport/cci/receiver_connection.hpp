//  Copyright (c) 2014-2015 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_PARCELSET_CCI_RECEIVER_CONNECTION_HPP
#define HPX_PARCELSET_CCI_RECEIVER_CONNECTION_HPP

namespace hpx { namespace parcelset { namespace policies { namespace cci
{
    class HPX_EXPORT parcelport;

    struct receiver_connection
    {
        typedef
            std::vector<char>
            data_type;
        typedef parcel_buffer<data_type, data_type> buffer_type;
        enum recv_type
        {
            recv_header = 1,
            recv_data = 2
        };

        receiver_connection(parcelport & pp)
          : pp_(pp)
        {
        }

        ~receiver_connection()
        {
        }

        void receive_header(cci_rma_handle_t const& send_handle, std::size_t size)
        {
            header_.reset(size, connection_->endpoint);

            std::cout << "receiving " << size << "\n";

            items_recvd_ = 0;
            CCI_ENSURE(
                cci_rma(
                    connection_
                  , NULL, 0
                  , &header_.this_handle(), 0
                  , &send_handle, 0
                  , size
                  , (void *)recv_header, CCI_FLAG_READ
                )
              , connection_->endpoint
            );
        }

        void receive_data()
        {
            buffer_.clear();
            buffer_.data_.resize(header_.size());
            cci_rma_handle_t * h = header_.data_handle();

            buffer_.num_chunks_ = header_.num_chunks();

            // determine the size of the chunk buffer
            std::size_t num_zero_copy_chunks =
                static_cast<std::size_t>(
                    static_cast<boost::uint32_t>(buffer_.num_chunks_.first));
            std::size_t num_non_zero_copy_chunks =
                static_cast<std::size_t>(
                    static_cast<boost::uint32_t>(buffer_.num_chunks_.second));

            num_items_ = 1 + num_zero_copy_chunks;
            std::cout << header_.size() << " " << num_zero_copy_chunks << "\n";

            CCI_ENSURE(
                cci_rma_register(
                    connection_->endpoint
                  , buffer_.data_.data()
                  , buffer_.data_.size()
                  , CCI_FLAG_WRITE
                  , &header_.this_data_handle_
                )
              , connection_->endpoint
            );
            CCI_ENSURE(
                cci_rma(
                    connection_
                  , NULL, 0
                  , header_.this_data_handle_, 0
                  , h, 0
                  , header_.size()
                  , (void *)recv_data, CCI_FLAG_READ
                )
              , connection_->endpoint
            );

            if(num_zero_copy_chunks != 0)
            {
                header_.rma_handles_.resize(num_zero_copy_chunks);
                buffer_.transmission_chunks_.resize(
                    num_zero_copy_chunks + num_non_zero_copy_chunks
                );
                std::memcpy(
                    buffer_.transmission_chunks_.data()
                  , header_.chunk_data()
                  , buffer_.transmission_chunks_.size()
                      * sizeof(buffer_type::transmission_chunk_type)
                );
                buffer_.chunks_.resize(num_zero_copy_chunks);

                for (std::size_t i = 0; i != num_zero_copy_chunks; ++i)
                {
                    std::size_t chunk_size = buffer_.transmission_chunks_[i].second;
                    buffer_.chunks_[i].resize(chunk_size);
                    CCI_ENSURE(
                        cci_rma_register(
                            connection_->endpoint
                          , buffer_.chunks_[i].data()
                          , chunk_size
                          , CCI_FLAG_WRITE
                          , &header_.rma_handles_[i]
                        )
                      , connection_->endpoint
                    );
                    CCI_ENSURE(
                        cci_rma(
                            connection_
                          , NULL, 0
                          , header_.rma_handles_[i], 0
                          , header_.chunk_handle(i), 0
                          , chunk_size
                          , (void *)recv_data, CCI_FLAG_READ
                        )
                      , connection_->endpoint
                    );
                }
            }
        }

        void receive_done()
        {
            ++items_recvd_;
            if (items_recvd_ == num_items_)
            {
                // deregistering data handles ...
//                 cci_rma_deregister(connection_->endpoint, data_handle_);
                header_.deregister();

                decode_parcels(pp_, std::move(buffer_), -1);
                message m(message::done());
                CCI_ENSURE(
                    cci_send(connection_, &m, sizeof(message), NULL, CCI_FLAG_SILENT)
                  , connection_->endpoint
                );
            }
        }

        parcelport& pp_;

        cci_connection_t* connection_;

        header header_;

        buffer_type buffer_;
        cci_rma_handle_t* data_handle_;
        std::size_t num_items_;
        boost::atomic<std::size_t> items_recvd_;
    };
}}}}

#endif
