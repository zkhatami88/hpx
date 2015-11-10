//  Copyright (c) 2014-2015 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_PARCELSET_CCI_LOCALITY_HPP
#define HPX_PARCELSET_CCI_LOCALITY_HPP

#include <hpx/runtime/serialization/serialization_fwd.hpp>
#include <hpx/util/safe_bool.hpp>

#include <string>

namespace hpx { namespace parcelset { namespace policies { namespace cci {
    class locality
    {
    public:
        locality()
        {}

        explicit locality(std::string uri)
          : uri_(uri)
        {}

        static const char *type()
        {
            return "cci";
        }

        operator util::safe_bool<locality>::result_type() const
        {
            return util::safe_bool<locality>()(!uri_.empty());
        }

        void save(serialization::output_archive & ar) const
        {
            ar << uri_;
        }

        void load(serialization::input_archive & ar)
        {
            ar >> uri_;
        }

        std::string const& uri() const
        {
            return uri_;
        }

    private:
        friend bool operator==(locality const & lhs, locality const & rhs)
        {
            return lhs.uri_ == rhs.uri_;
        }

        friend bool operator<(locality const & lhs, locality const & rhs)
        {
            return lhs.uri_ < rhs.uri_;
        }

        friend std::ostream & operator<<(std::ostream & os, locality const & loc)
        {
            os << loc.uri_;
            return os;
        }

        std::string uri_;
    };
}}}}

#endif
