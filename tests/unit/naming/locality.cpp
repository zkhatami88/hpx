//  Copyright (c) 2015 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_main.hpp>
#include <hpx/hpx_fwd.hpp>

#include <hpx/async.hpp>
#include <hpx/apply.hpp>

#include <hpx/runtime/naming/locality.hpp>
#include <hpx/runtime/actions/plain_action.hpp>

#include <hpx/util/lightweight_test.hpp>

hpx::naming::locality f1()
{
    return hpx::naming::locality(hpx::get_locality_id());
}

hpx::naming::locality f2(int)
{
    return hpx::naming::locality(hpx::get_locality_id());
}

HPX_PLAIN_ACTION(f1);
HPX_PLAIN_ACTION(f2);

int main()
{
    std::size_t num_localities = hpx::get_num_localities_sync();

    for(std::size_t i = 0; i < num_localities; ++i)
    {
        f1_action act1;
        f2_action act2;

        hpx::naming::locality l(i);
        hpx::apply(l, act1);
        hpx::apply(l, act2, 5);

        HPX_TEST_EQ(hpx::async(l, act1).get(), l);
        HPX_TEST_EQ(hpx::async(l, act2, 5).get(), l);
    }

    hpx::naming::locality l = hpx::naming::id_type();
    hpx::naming::id_type i = l;

    return hpx::util::report_errors();
}
