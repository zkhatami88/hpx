//  Copyright (c) 2015 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This must fail compiling

#include <hpx/hpx_main.hpp>
#include <hpx/include/actions.hpp>

void test (int* ptr) {}
HPX_PLAIN_ACTION(test);

///////////////////////////////////////////////////////////////////////////////
int main()
{
    int* ptr = 0;

    test_action act;
    hpx::apply(act, hpx::find_here(), ptr);

    return 0;
}

