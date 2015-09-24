//  Copyright (c) 2015 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_NAMING_LOCALITY_HPP
#define HPX_NAMING_LOCALITY_HPP

#include <hpx/config/defines.hpp>

#include <hpx/parallel/executors/executor_traits.hpp>

#include <hpx/async.hpp>
#include <hpx/apply.hpp>
#include <hpx/runtime/actions/plain_action.hpp>
#include <hpx/runtime/naming/name.hpp>
#if defined(HPX_HAVE_ID_TYPE_COMPATIBILITY)
#include <hpx/runtime/naming/id_type.hpp>
#endif

#include <hpx/runtime/serialization/serialization_fwd.hpp>
#include <hpx/util/decay.hpp>
#include <hpx/util/detail/pack.hpp>

#include <hpx/traits/is_action.hpp>
#include <hpx/traits/is_bitwise_serializable.hpp>


#include <iosfwd>

namespace hpx { namespace naming {

    /// locality is the representation of given locality in AGAS
    class HPX_EXPORT locality : hpx::parallel::executor_tag
    {
        template <typename Action, typename Tuple, std::size_t...Is>
        void apply_impl(
            Action act, id_type id, Tuple && args, util::detail::pack_c<std::size_t, Is...>)
        {
            hpx::apply(act, id, std::move(util::get<Is>(args))...);
        }

        template <typename Action, typename Tuple, std::size_t...Is>
        lcos::future<
            typename hpx::actions::extract_action<
                Action
            >::type::local_result_type
        >
        async_impl(
            Action act, id_type id, Tuple && args, util::detail::pack_c<std::size_t, Is...>)
        {
            return
                hpx::async(act, id, std::move(util::get<Is>(args))...);
        }
    public:

        locality()
          : gid_(get_gid_from_locality_id(invalid_locality_id))
        {}

        explicit locality(boost::uint32_t locality_id)
          : gid_(get_gid_from_locality_id(locality_id))
        {
        }

        static locality here()
        {
            return locality(hpx::get_locality_id());
        }

#if defined(HPX_HAVE_ID_TYPE_COMPATIBILITY)
        locality(id_type id)
          : gid_(id.get_gid())
        {
            HPX_ASSERT(id.get_management_type() == id_type::unmanaged);
            HPX_ASSERT(is_locality(id));
        }

        operator id_type() const
        {
            return id_type(gid_, id_type::unmanaged);
        }
#endif

        boost::uint32_t id() const
        {
            return get_locality_id_from_gid(gid_);
        }

        /// \cond NOINTERNAL
        typedef hpx::parallel::parallel_execution_tag execution_category;

        template <typename F>
        typename std::enable_if<
            !traits::is_action<F>::value
          , void
        >::type apply_execute(F && f)
        {
            typedef typename util::decay<F>::type func_type;
            typedef typename func_type::func_type action_type;
            typedef typename func_type::args_type args_type;
            static_assert(
                traits::is_action<action_type>::value
              , "Only actions can be executed on localities"
            );
            static_assert(
                std::is_same<
                    typename action_type::component_type
                  , hpx::actions::detail::plain_function
                >::value
              , "Only plain actions can be executed on a locality"
            );

            HPX_ASSERT(id() != invalid_locality_id);

            // FIXME: apply should take the gid directly ...
            apply_impl(
                std::move(f._f)
              , id_type(gid_, id_type::unmanaged)
              , f._args
              , typename util::detail::make_index_pack<
                    util::tuple_size<args_type>::value
                >::type()
            );
        }

        template <typename F>
        typename std::enable_if<
            traits::is_action<F>::value
          , void
        >::type apply_execute(F && f)
        {
            typedef typename util::decay<F>::type action_type;
            static_assert(
                traits::is_action<action_type>::value
              , "Only actions can be executed on localities"
            );
            static_assert(
                std::is_same<
                    typename action_type::component_type
                  , hpx::actions::detail::plain_function
                >::value
              , "Only plain actions can be executed on a locality"
            );
            HPX_ASSERT(id() != invalid_locality_id);

            // FIXME: apply should take the gid directly ...
            apply_impl(
                std::forward<F>(f)
              , id_type(gid_, id_type::unmanaged)
              , hpx::util::tuple<>()
              , util::detail::make_index_pack<0>::type()
            );
        }

        template <typename F, typename Enable = void>
        struct async_result;

        template <typename F>
        struct async_result<F, typename std::enable_if<traits::is_action<F>::value>::type>
        {
            typedef
                lcos::future<
                    typename hpx::actions::extract_action<
                        typename hpx::util::decay<F>::type
                    >::type::local_result_type
                >
                type;
        };

        template <typename F>
        typename std::enable_if<
            traits::is_action<F>::value
          , typename async_result<F>::type
        >::type async_execute(F && f)
        {
            typedef typename util::decay<F>::type action_type;
            static_assert(
                traits::is_action<action_type>::value
              , "Only actions can be executed on localities"
            );
            static_assert(
                std::is_same<
                    typename action_type::component_type
                  , hpx::actions::detail::plain_function
                >::value
              , "Only plain actions can be executed on a locality"
            );
            HPX_ASSERT(id() != invalid_locality_id);

            // FIXME: async should take the gid directly ...
            return async_impl(
                std::forward<F>(f)
              , id_type(gid_, id_type::unmanaged)
              , hpx::util::tuple<>()
              , util::detail::make_index_pack<0>::type()
            );
        }

        template <typename F>
        struct async_result<F, typename std::enable_if<!traits::is_action<F>::value>::type>
        {
            typedef
                lcos::future<
                    typename hpx::actions::extract_action<
                        typename hpx::util::decay<F>::type::func_type
                    >::type::local_result_type
                >
                type;
        };

        template <typename F>
        typename std::enable_if<
            !traits::is_action<F>::value
          , typename async_result<F>::type
        >::type async_execute(F && f)
        {
            typedef typename util::decay<F>::type func_type;
            typedef typename func_type::func_type action_type;
            typedef typename func_type::args_type args_type;
            static_assert(
                traits::is_action<action_type>::value
              , "Only actions can be executed on localities"
            );
            static_assert(
                std::is_same<
                    typename action_type::component_type
                  , hpx::actions::detail::plain_function
                >::value
              , "Only plain actions can be executed on a locality"
            );
            HPX_ASSERT(id() != invalid_locality_id);

            // FIXME: async should take the gid directly ...
            return async_impl(
                std::move(f._f)
              , id_type(gid_, id_type::unmanaged)
              , f._args
              , typename util::detail::make_index_pack<
                    util::tuple_size<args_type>::value
                >::type()
            );
        }
        /// \endcond

    private:
        gid_type gid_;

        friend class hpx::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, unsigned)
        {
            ar & gid_;
        }
    };

    inline bool operator==(locality const & l1, locality const & l2)
    {
        return l1.id() == l2.id();
    }

#if defined(HPX_HAVE_ID_TYPE_COMPATIBILITY)
    inline bool operator==(id_type const & l1, locality const & l2)
    {
        return l1 == id_type(l2);
    }

    inline bool operator==(locality const & l1, id_type const & l2)
    {
        return id_type(l1) == l2;
    }
#endif

    inline bool operator!=(locality const & l1, locality const & l2)
    {
        return l1.id() != l2.id();
    }

#if defined(HPX_HAVE_ID_TYPE_COMPATIBILITY)
    inline bool operator!=(id_type const & l1, locality const & l2)
    {
        return l1 != id_type(l2);
    }

    inline bool operator!=(locality const & l1, id_type const & l2)
    {
        return id_type(l1) == l2;
    }
#endif

    inline bool operator<=(locality const & l1, locality const & l2)
    {
        return l1.id() <= l2.id();
    }

    inline bool operator>=(locality const & l1, locality const & l2)
    {
        return l1.id() >= l2.id();
    }

    inline bool operator<(locality const & l1, locality const & l2)
    {
        return l1.id() < l2.id();
    }

    inline bool operator>(locality const & l1, locality const & l2)
    {
        return l1.id() > l2.id();
    }

    inline std::ostream &operator<<(std::ostream &os, locality const & l)
    {
        os << "locality#" << l.id();
        return os;
    }
}}

HPX_IS_BITWISE_SERIALIZABLE(hpx::naming::locality);

#endif
