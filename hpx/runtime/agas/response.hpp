////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2011 Bryce Adelstein-Lelbach
//  Copyright (c) 2014-2016 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////

#ifndef HPX_RUNTIME_AGAS_RESPONSE_HPP
#define HPX_RUNTIME_AGAS_RESPONSE_HPP

#include <hpx/config.hpp>
#include <hpx/exception_fwd.hpp>
#include <hpx/throw_exception.hpp>
#include <hpx/lcos/base_lco_with_value.hpp>
#include <hpx/runtime/actions/basic_action.hpp>
#include <hpx/runtime/agas/gva.hpp>
#include <hpx/runtime/agas/namespace_action_code.hpp>
#include <hpx/runtime/components/component_type.hpp>
#include <hpx/runtime/naming/address.hpp>
#include <hpx/runtime/naming/id_type.hpp>
#include <hpx/runtime/naming/name.hpp>
#include <hpx/runtime/parcelset/locality.hpp>
#include <hpx/traits/get_remote_result.hpp>
#include <hpx/runtime/serialization/serialization_fwd.hpp>

#include <cstdint>
#include <map>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace hpx { namespace agas
{

// TODO: Ensure that multiple invocations of get_data get optimized into the
// same jump table.
struct HPX_EXPORT response
{
  private:
    struct response_data;
  public:
    response();

    response(
        namespace_action_code type_
      , naming::gid_type lower_
      , naming::gid_type upper_
      , std::uint32_t prefix_
      , error status_ = success
        );

    response(
        namespace_action_code type_
      , naming::gid_type const& gidbase_
      , gva const& gva_
      , naming::gid_type const& locality_
      , error status_ = success
        );

    response(
        namespace_action_code type_
      , gva const& gva_
      , naming::gid_type const& locality_
      , error status_ = success
        );

    response(
        namespace_action_code type_
      , components::component_type ctype_
      , error status_ = success
        );

    response(
        namespace_action_code type_
      , std::vector<std::uint32_t> const& prefixes_
      , error status_ = success
        );

    response(
        namespace_action_code type_
      , naming::gid_type gid_
      , error status_ = success
        );

    response(
        namespace_action_code type_
      , std::uint32_t prefix_
      , error status_ = success
        );

    response(
        namespace_action_code type_
      , std::string const& name_
      , error status_ = success
        );

    explicit response(
        namespace_action_code type_
      , error status_ = success
        );

    response(
        namespace_action_code type_
      , std::map<naming::gid_type, parcelset::endpoints_type> const & localities_
      , error status_ = success
        );

    response(
        namespace_action_code type_
      , std::int64_t added_credits_
      , error status_ = success
        );

    response(
        namespace_action_code type_
      , parcelset::endpoints_type const& endpoints_
      , error status_ = success
        );

    ~response();

    ///////////////////////////////////////////////////////////////////////////
    // copy constructor
    response(
        response const& other
        );

    // copy assignment
    response& operator=(
        response const& other
        );

    gva get_gva(
        error_code& ec = throws
        ) const;

    std::vector<std::uint32_t> get_localities(
        error_code& ec = throws
        ) const;

    std::map<naming::gid_type, parcelset::endpoints_type> get_resolved_localities(
        error_code& ec = throws
        ) const;

    parcelset::endpoints_type get_endpoints(
        error_code& ec = throws
        ) const;

    std::uint32_t get_num_localities(
        error_code& ec = throws
        ) const;

    std::uint32_t get_num_overall_threads(
        error_code& ec = throws
        ) const;

    std::vector<std::uint32_t> get_num_threads(
        error_code& ec = throws
        ) const;

    components::component_type get_component_type(
        error_code& ec = throws
        ) const;

    std::uint32_t get_locality_id(
        error_code& ec = throws
        ) const;

    naming::gid_type get_locality(
        error_code& ec = throws
        ) const;

    // primary_ns_change_credit_one
    std::int64_t get_added_credits(
        error_code& ec = throws
        ) const;

    naming::gid_type get_base_gid(
        error_code& ec = throws
        ) const;

    naming::gid_type get_gid(
        error_code& ec = throws
        ) const;

    naming::gid_type get_lower_bound(
        error_code& ec = throws
        ) const;

    naming::gid_type get_upper_bound(
        error_code& ec = throws
        ) const;

    std::string get_component_typename(
        error_code& ec = throws
        ) const;

    naming::gid_type get_statistics_counter(
        error_code& ec = throws
        ) const;

    namespace_action_code get_action_code() const
    {
        return mc;
    }

    error get_status() const
    {
        return status;
    }

  private:
    friend class hpx::serialization::access;

    void save(
        serialization::output_archive&
      , const unsigned int
        ) const;

    void load(
        serialization::input_archive&
      , const unsigned int
        );

    HPX_SERIALIZATION_SPLIT_MEMBER()

    namespace_action_code mc; //-V707
    error status;
    std::unique_ptr<response_data> data;
};

template <typename Result>
struct get_response_result
{
    static agas::response const& call(agas::response const& rep)
    {
        return rep;
    }
};

// TODO: verification of namespace_action_code
template <>
struct get_response_result<naming::id_type>
{
    static naming::id_type call(
        agas::response const& rep
        )
    {
        switch(rep.get_action_code()) {
        case agas::symbol_ns_unbind:
        case agas::symbol_ns_resolve:
        case agas::primary_ns_statistics_counter:
        case agas::component_ns_statistics_counter:
        case agas::symbol_ns_statistics_counter:
        case agas::primary_ns_resolve_gid:
            {
                naming::gid_type raw_gid = rep.get_gid();

                if (naming::detail::has_credits(raw_gid))
                    return naming::id_type(raw_gid, naming::id_type::managed);

                return naming::id_type(raw_gid, naming::id_type::unmanaged);
            }

        default:
            break;
        }

        HPX_THROW_EXCEPTION(bad_parameter,
            "get_response_result<naming::id_type, agas::response>::call",
            "unexpected action code in result conversion");
        return naming::invalid_id;
    }
};

template <>
struct get_response_result<std::uint32_t>
{
    static std::uint32_t call(
        agas::response const& rep
        )
    {
        switch(rep.get_action_code()) {
        case agas::locality_ns_num_localities:
        case agas::component_ns_num_localities:
            return rep.get_num_localities();

        case agas::locality_ns_num_threads:
            return rep.get_num_overall_threads();

        default:
            break;
        }
        HPX_THROW_EXCEPTION(bad_parameter,
            "get_response_result<std::uint32_t, agas::response>::call",
            "unexpected action code in result conversion");
        return 0;
    }
};

template <>
struct get_response_result<std::int64_t>
{
    static std::int64_t call(
        agas::response const& rep
        )
    {
        switch(rep.get_action_code()) {
        case agas::primary_ns_increment_credit:
            return rep.get_added_credits();

        default:
            break;
        }

        HPX_THROW_EXCEPTION(bad_parameter,
            "get_response_result<std::int64_t, agas::response>::call",
            "unexpected action code in result conversion");
        return 0;
    }
};

template <>
struct get_response_result<bool>
{
    static bool call(
        agas::response const& rep
        )
    {
        switch(rep.get_action_code()) {
        case agas::symbol_ns_bind:
        case agas::symbol_ns_on_event:
        case agas::primary_ns_end_migration:
            return rep.get_status() == success;

        default:
            break;
        }

        HPX_THROW_EXCEPTION(bad_parameter,
            "get_response_result<void, agas::response>::call",
            "unexpected action code in result conversion");
        return false;
    }
};

template <>
struct get_response_result<std::vector<std::uint32_t>>
{
    static std::vector<std::uint32_t> call(
        agas::response const& rep
        )
    {
        switch(rep.get_action_code()) {
        case agas::locality_ns_num_threads:
            return rep.get_num_threads();

        default:
            break;
        }

        HPX_THROW_EXCEPTION(bad_parameter,
            "get_response_result<std::vector<std::uint32_t>, agas::response>::call",
            "unexpected action code in result conversion");
        return std::vector<std::uint32_t>();
    }
};

template <>
struct get_response_result<naming::address>
{
    static naming::address call(
        agas::response const& rep
        )
    {
        switch(rep.get_action_code()) {
        case agas::primary_ns_unbind_gid:
            {
                agas::gva g = rep.get_gva();
                return naming::address(g.prefix, g.type, g.lva());
            }

        default:
            break;
        }

        HPX_THROW_EXCEPTION(bad_parameter,
            "get_response_result<naming::address>, agas::response>::call",
            "unexpected action code in result conversion");
        return naming::address();
    }
};

template <>
struct get_response_result<std::pair<naming::id_type, naming::address> >
{
    static std::pair<naming::id_type, naming::address> call(
        agas::response const& rep
        )
    {
        switch(rep.get_action_code()) {
        case agas::primary_ns_begin_migration:
            {
                agas::gva g = rep.get_gva();

                naming::address addr(g.prefix, g.type, g.lva());
                naming::id_type loc(rep.get_locality(), id_type::unmanaged);
                return std::pair<naming::id_type, naming::address>(loc, addr);
            }

        default:
            break;
        }

        HPX_THROW_EXCEPTION(bad_parameter,
            "get_response_result<std::pair<naming::id_type, naming::address>,"
            " agas::response>::call",
            "unexpected action code in result conversion");
        return std::pair<naming::id_type, naming::address>();
    }
};

template <>
struct get_response_result<std::map<naming::gid_type, parcelset::endpoints_type> >
{
    static std::map<naming::gid_type, parcelset::endpoints_type> call(
        agas::response const& rep
        )
    {
        return rep.get_resolved_localities();
    }
};

template <>
struct get_response_result<parcelset::endpoints_type>
{
    static parcelset::endpoints_type call(
        agas::response const& rep
        )
    {
        return rep.get_endpoints();
    }
};

}}

HPX_REGISTER_BASE_LCO_WITH_VALUE_DECLARATION(
    hpx::agas::response, hpx_agas_response_type)

HPX_REGISTER_BASE_LCO_WITH_VALUE_DECLARATION(
    std::vector<hpx::agas::response>, hpx_agas_response_vector_type)

#endif /*HPX_RUNTIME_AGAS_RESPONSE_HPP*/
