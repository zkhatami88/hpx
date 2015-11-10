//  Copyright (c) 2014-2015 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/config/defines.hpp>
#include <hpx/config/warnings_prefix.hpp>

#if defined(HPX_HAVE_PARCELPORT_CCI)
#include <cci.h>
#endif

#include <hpx/plugins/parcelport_factory.hpp>
#include <hpx/util/command_line_handling.hpp>

#include <hpx/runtime.hpp>
#include <hpx/runtime/parcelset/decode_parcels.hpp>
#include <hpx/runtime/parcelset/locality.hpp>
#include <hpx/runtime/parcelset/parcelport_impl.hpp>
#include <hpx/runtime/parcelset/parcelport_connection.hpp>
#include <hpx/runtime/parcelset/detail/call_for_each.hpp>
#include <hpx/util/runtime_configuration.hpp>
#include <hpx/util/safe_lexical_cast.hpp>

#include <hpx/plugins/parcelport/cci/header.hpp>
#include <hpx/plugins/parcelport/cci/locality.hpp>
#include <hpx/plugins/parcelport/cci/message.hpp>
#include <hpx/plugins/parcelport/cci/sender_connection.hpp>
#include <hpx/plugins/parcelport/cci/receiver_connection.hpp>

namespace hpx { namespace parcelset
{
    namespace policies { namespace cci
    {
        class HPX_EXPORT parcelport;

    }}

    template <>
    struct connection_handler_traits<policies::cci::parcelport>
    {
        typedef policies::cci::sender_connection connection_type;
        typedef boost::mpl::false_  send_early_parcel;
        typedef boost::mpl::true_ do_background_work;

        static const char * type()
        {
            return "cci";
        }

        static const char * pool_name()
        {
            return "parcel-pool-cci";
        }

        static const char * pool_name_postfix()
        {
            return "-cci";
        }
    };

    namespace policies { namespace cci
    {
        class HPX_EXPORT parcelport
          : public parcelport_impl<parcelport>
        {
            static parcelset::locality here()
            {
                return
                    parcelset::locality(
                        locality()
                    );
            }
            typedef parcelport_impl<parcelport> base_type;
            typedef hpx::lcos::local::spinlock mutex_type;

        public:
            parcelport(util::runtime_configuration const& ini,
                util::function_nonser<void(std::size_t, char const*)> const& on_start,
                util::function_nonser<void()> const& on_stop)
              : base_type(ini, here(), on_start, on_stop)
              , stopped_(false)
            {
                CCI_ENSURE(cci_create_endpoint(NULL, 0, &endpoint_, &fd_), NULL);
                char* uri = NULL;
                CCI_ENSURE(cci_get_opt(endpoint_, CCI_OPT_ENDPT_URI, &uri), endpoint_);
                uri_ = uri;
                here_ =
                    parcelset::locality(
                        locality(uri_)
                    );
            }

            ~parcelport()
            {
                cci_destroy_endpoint(endpoint_);
            }

            /// Start the handling of connections.
            bool do_run()
            {

                for(std::size_t i = 0; i != io_service_pool_.size(); ++i)
                {
                    io_service_pool_.get_io_service(i).post(
                        [this]()
                        {
                            while(!stopped_)
                            {
                                poll_events(true);
                            }
                        }
                    );
                }

                return true;
            }

            /// Stop the handling of connectons.
            void do_stop()
            {
                stopped_ = true;
            }

            /// Return the name of this locality
            std::string get_locality_name() const
            {
                return uri_;
            }

            boost::shared_ptr<sender_connection> create_connection(
                parcelset::locality const& l, error_code& ec)
            {
                boost::shared_ptr<sender_connection> connection_ptr
                    = boost::make_shared<sender_connection>(endpoint_, l);
                while(connection_ptr->connection_.load() == 0)
                {
                    poll_events(false);
                }
                return connection_ptr;
            }

            parcelset::locality agas_locality(
                util::runtime_configuration const & ini) const
            {
                HPX_ASSERT(false);
                return parcelset::locality();
            }

            parcelset::locality create_locality() const
            {
                return parcelset::locality(locality());
            }

            bool background_work(std::size_t num_thread)
            {
                return poll_events(false);
            }

        private:

            bool poll_events(bool blocking)
            {
                int ret;
                if(blocking)
                {
                    timeval timeout = {0, 10};
                    fd_set rfds;
                    FD_ZERO(&rfds);
                    FD_SET(fd_, &rfds);
                    ret = select(1, &rfds, NULL, NULL, &timeout);
                    if(!ret) return false;
                }

                cci_event_t *event;
                ret = cci_get_event(endpoint_, &event);
                if(ret == CCI_SUCCESS)
                {
                    switch (event->type)
                    {
                        // Recieved a message
                        case CCI_EVENT_RECV:
                            {
                                HPX_ASSERT(event->recv.len == sizeof(message));
                                message m;
                                std::memcpy(&m, event->recv.ptr, sizeof(message));
                                switch (m.type_)
                                {
                                    case message::send_rma_handle:
                                    {
                                        auto connection_ptr =
                                            static_cast<receiver_connection*>(
                                                event->recv.connection->context
                                            );
                                        connection_ptr->receive_header(m.rma_handle_, m.size_);
                                        break;
                                    }
                                    case message::send_done:
                                    {
                                        auto connection_ptr =
                                            static_cast<sender_connection*>(
                                                event->recv.connection->context
                                            );
                                        connection_ptr->done();
                                        break;
                                    }
                                    case message::send_disconnect:
                                    {
                                        boost::unique_lock<mutex_type>
                                            l(accepted_connections_mtx_);
                                        auto it = std::find_if(
                                            accepted_connections_.begin()
                                          , accepted_connections_.end()
                                          , [this, &event](receiver_connection const & r)
                                            {
                                                return
                                                    r.connection_ == event->recv.connection;
                                            }
                                        );
                                        HPX_ASSERT(it != accepted_connections_.end());
                                        accepted_connections_.erase(it);
                                        break;
                                    }
                                    default:
                                        // No valid message ...
                                        HPX_ASSERT(false);
                                }
                                break;
                            }
                        case CCI_EVENT_SEND:
                            if(event->send.status == CCI_SUCCESS)
                            {
                                receiver_connection::recv_type type;
                                std::memcpy(&type, &event->send.context, sizeof(type));
                                auto connection_ptr =
                                    static_cast<receiver_connection*>(
                                        event->send.connection->context
                                    );
                                switch (type)
                                {
                                    case receiver_connection::recv_header:
                                        connection_ptr->receive_data();
                                        break;
                                    case receiver_connection::recv_data:
                                        connection_ptr->receive_done();
                                        break;
                                    default:
                                        HPX_ASSERT(false);
                                }
                            }
                            break;
                        // client connected
                        case CCI_EVENT_CONNECT:
                            if(event->connect.status == CCI_SUCCESS)
                            {
                                auto connection_ptr =
                                    static_cast<sender_connection*>(
                                        event->connect.context
                                    );
                                connection_ptr->connection_
                                    .store(event->connect.connection);
                            }
                            break;
                        // new incoming connect request
                        case CCI_EVENT_CONNECT_REQUEST:
                            {
                                receiver_connection* context;
                                {
                                    boost::unique_lock<mutex_type>
                                        l(accepted_connections_mtx_);
                                    accepted_connections_.emplace_back(*this);
                                    context = &accepted_connections_.back();
                                }
                                CCI_ENSURE(cci_accept(event, context), endpoint_);
                                break;
                            }
                        // new accepted connection
                        case CCI_EVENT_ACCEPT:
                            {
                                auto connection_ptr =
                                    static_cast<receiver_connection*>(
                                        event->accept.context);
                                connection_ptr->connection_ = event->accept.connection;
                                break;
                            }
                        default:
                            break;
                    }
                    cci_return_event(event);
                    return true;
                }

                return false;
            }

            cci_os_handle_t fd_;
            cci_endpoint_t *endpoint_;
            std::string uri_;

            boost::atomic<bool> stopped_;

            mutex_type accepted_connections_mtx_;
            std::list<receiver_connection> accepted_connections_;
        };
    }
}}}

#include <hpx/config/warnings_suffix.hpp>

namespace hpx { namespace traits
{
    // Inject additional configuration data into the factory registry for this
    // type. This information ends up in the system wide configuration database
    // under the plugin specific section:
    //
    //      [hpx.parcel.cci]
    //      ...
    //      priority = 100
    //
    template <>
    struct plugin_config_data<hpx::parcelset::policies::cci::parcelport>
    {
        static char const* priority()
        {
            return "1000";
        }
        static void init(int *argc, char ***argv, util::command_line_handling &cfg)
        {
            boost::uint32_t caps = 0;
            CCI_ENSURE(cci_init(CCI_ABI_VERSION, 0, &caps), NULL);
        }

        static char const* call()
        {
            return "";
        }
    };
}}

HPX_REGISTER_PARCELPORT(
    hpx::parcelset::policies::cci::parcelport,
    cci);
