//  Copyright (c) 2016 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_TRAITS_FUTURE_ITERATOR_TRAITS_MAR_22_2016_0201PM)
#define HPX_TRAITS_FUTURE_ITERATOR_TRAITS_MAR_22_2016_0201PM

#include <hpx/traits.hpp>
#include <hpx/traits/future_traits.hpp>
#include <hpx/util/always_void.hpp>

#include <iterator>

namespace hpx { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename Iter, typename Enable>
    struct future_iterator_traits
    {};

    template <typename Iterator>
    struct future_iterator_traits<Iterator,
        typename hpx::util::always_void<
#if defined(HPX_MSVC) && HPX_MSVC <= 1800       // MSVC12 needs special help
            typename Iterator::iterator_category
#else
            typename std::iterator_traits<Iterator>::value_type
#endif
        >::type>
    {
        typedef
            typename std::iterator_traits<Iterator>::value_type
            type;

        typedef future_traits<type> traits_type;
    };
}}

#endif
