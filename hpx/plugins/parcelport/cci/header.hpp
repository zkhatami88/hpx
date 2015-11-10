//  Copyright (c) 2014-2015 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_PARCELSET_CCI_HEADER_HPP
#define HPX_PARCELSET_CCI_HEADER_HPP

#include <cci.h>

#define CCI_ENSURE(func, endpoint)                                             \
{                                                                              \
    cci_status_t ret = static_cast<cci_status_t>(func);                        \
    if(ret != CCI_SUCCESS)                                                     \
    {                                                                          \
        std::cout                                                              \
            << BOOST_PP_STRINGIZE(func) << " failed: "                         \
            << cci_strerror(endpoint, ret) << "\n";                            \
        std::terminate();                                                      \
    }                                                                          \
}                                                                              \
/**/

namespace hpx { namespace parcelset {  namespace policies { namespace cci
{
        struct header
        {
            typedef boost::uint32_t value_type;
            enum data_pos
            {
                pos_tag              = 0 * sizeof(value_type),
                pos_size             = 1 * sizeof(value_type),
                pos_numbytes         = 2 * sizeof(value_type),
                pos_numchunks_first  = 3 * sizeof(value_type),
                pos_numchunks_second = 4 * sizeof(value_type),
                pos_rma_handle       = 5 * sizeof(value_type)
            };

            header()
              : endpoint_(0)
              , this_data_handle_(0)
            {}

            ~header()
            {
                deregister();
            }

            void deregister()
            {
                if(endpoint_)
                {
                    if(this_data_handle_)
                    {
                        cci_rma_deregister(endpoint_, this_data_handle_);
                        this_data_handle_ = 0;
                    }
                    for(cci_rma_handle_t* handle : rma_handles_)
                    {
                        cci_rma_deregister(endpoint_, handle);
                    }
                    rma_handles_.clear();
                    endpoint_ = 0;
                }
            }

            void reset(std::size_t size, cci_endpoint_t* endpoint)
            {
                HPX_ASSERT(this_data_handle_ == 0);
                HPX_ASSERT(rma_handles_.empty());
                HPX_ASSERT(endpoint_ == 0);
                endpoint_ = endpoint;

                data_.resize(size);
                CCI_ENSURE(
                    cci_rma_register(
                        endpoint_
                      , data_.data()
                      , data_.size()
                      , CCI_FLAG_WRITE
                      , &this_data_handle_
                    )
                  , endpoint_
                );
            }

            template <typename Buffer>
            void reset(Buffer& buffer, cci_endpoint_t* endpoint)
            {
                HPX_ASSERT(this_data_handle_ == 0);
                HPX_ASSERT(rma_handles_.empty());
                HPX_ASSERT(endpoint_ == 0);
                endpoint_ = endpoint;

                std::size_t num_chunks = buffer.num_chunks_.first;
                num_chunks++;
                auto& chunks = buffer.transmission_chunks_;

                data_.resize(
                    5 * sizeof(value_type)
                  + num_chunks * sizeof(cci_rma_handle_t)
                  + chunks.size()
                        * sizeof(typename Buffer::transmission_chunk_type)
                );
                rma_handles_.resize(num_chunks);

                set<pos_size>(buffer.size_);
                set<pos_numbytes>(buffer.data_size_);
                set<pos_numchunks_first>(buffer.num_chunks_.first);
                set<pos_numchunks_second>(buffer.num_chunks_.second);

                CCI_ENSURE(
                    cci_rma_register(
                        endpoint_
                      , buffer.data_.data()
                      , buffer.data_.size()
                      , CCI_FLAG_READ
                      , &rma_handles_[0]
                    )
                  , endpoint_
                );
                set<pos_rma_handle>(*rma_handles_[0]);

                std::size_t idx = 1;
                if (!chunks.empty()) {
                    // now add chunks themselves, those hold zero-copy serialized chunks
                    for (serialization::serialization_chunk& c : buffer.chunks_)
                    {
                        if (c.type_ == serialization::chunk_type_pointer)
                        {
                            CCI_ENSURE(
                                cci_rma_register(
                                    endpoint_
                                  , const_cast<void *>(c.data_.cpos_)
                                  , c.size_
                                  , CCI_FLAG_READ
                                  , &rma_handles_[idx]
                                )
                              , endpoint_
                            );
                            ++idx;
                        }
                    }
                }

                std::memcpy(&data_[idx], chunks.data(), chunks.size()
                            * sizeof(typename Buffer::transmission_chunk_type));

                CCI_ENSURE(
                    cci_rma_register(
                        endpoint_
                      , data_.data()
                      , data_.size()
                      , CCI_FLAG_READ
                      , &this_data_handle_
                    )
                  , endpoint_
                  );
            }

            cci_rma_handle_t const& this_handle() const
            {
                return *this_data_handle_;
            }

            value_type size() const
            {
                return get<pos_size, value_type>();
            }

            value_type numbytes() const
            {
                return get<pos_numbytes, value_type>();
            }

            std::pair<value_type, value_type> num_chunks() const
            {
                return std::make_pair(get<pos_numchunks_first, value_type>(),
                        get<pos_numchunks_second, value_type>());
            }

            const char * chunk_data() const
            {
                std::size_t offset = sizeof(cci_rma_handle_t) * (num_chunks().first + 1);
                return &data_[pos_rma_handle + offset];
            }

            cci_rma_handle_t* data_handle() const
            {
                return reinterpret_cast<cci_rma_handle_t *>(&data_[pos_rma_handle]);
            }

            cci_rma_handle_t* chunk_handle(std::size_t idx) const
            {
                std::size_t offset = sizeof(cci_rma_handle_t) * (idx + 1);
                return reinterpret_cast<cci_rma_handle_t *>(&data_[pos_rma_handle + offset]);
            }

            std::vector<char> data_;
            cci_endpoint_t* endpoint_;
            cci_rma_handle_t* this_data_handle_;
            std::vector<cci_rma_handle_t*> rma_handles_;
        private:

            template <std::size_t Pos, typename T>
            void set(T const & t)
            {
                std::memcpy(&data_[Pos], &t, sizeof(t));
            }

            template <std::size_t Pos, typename T>
            void set(T const & t, std::size_t offset)
            {
                std::size_t offset_ = (sizeof(T)/sizeof(value_type)) * offset;
                std::memcpy(&data_[Pos] + offset_, &t, sizeof(t));
            }

            template <std::size_t Pos, typename T>
            T get() const
            {
                T res;
                std::memcpy(&res, &data_[Pos], sizeof(T));
                return res;
            }
        };
}}}}

#endif
