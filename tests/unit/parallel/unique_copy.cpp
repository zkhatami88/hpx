//  Copyright (c) 2015 Daniel Bourgeois
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/include/parallel_unique.hpp>
#include <hpx/include/parallel_partition.hpp>
#include <hpx/include/parallel_copy.hpp>
#include <hpx/include/parallel_scan.hpp>
#include <hpx/util/lightweight_test.hpp>

#include "test_utils.hpp"

int hpx_main(boost::program_options::variables_map& vm)
{
    using namespace hpx::parallel;

//HPX_ASSERT(workitems.size() == 2) from scan_partitioner
    std::vector<int> a{1,1};
    std::vector<int> b(a.size());
    exclusive_scan(par(task), a.begin(), a.end(), b.begin(), 100);
//

/*
//    for(int i = 0; i < 5000; ++i){ a.push_back(i); a.push_back(i); }
//        auto f2 = copy_if(par(task), a.begin(), a.end(), b.begin(), [](int qwe){ return true; });
//    auto f1 = unique_copy(par(task), a.begin(), a.end(), b.begin());
//    if (f1.get() == b.begin() && f2.get() == b.begin()) std::cout << "GOOD TO GO!\n";
//    if (f2.get() == b.begin()) std::cout << "copy_if is good!\n";
//    if (f1.get() == b.begin()) std::cout << "unique_copy good at zero length!\n";

    std::vector<int> a;
    for(int i = 0; i < 5000; ++i){ a.push_back(i); }
    std::vector<int> a_true(a.size()), a_false(a.size());
    auto f = partition(par(task), a.begin(), a.end(),
        a_true.begin(), a_false.begin(),
        [](int asd){ return asd % 2 == 0; });
    f.wait();

    for(int z : a_true) std::cout << z << " "; std::cout << std::endl;
    for(int z : a_false) std::cout << z << " "; std::cout << std::endl;
*/
    for(int z : b) std::cout << z << " "; std::cout << std::endl;
    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    // add command line option which controls the random number generator seed
    using namespace boost::program_options;
    options_description desc_commandline(
        "Usage: " HPX_APPLICATION_STRING " [options]");

    desc_commandline.add_options()
        ("seed,s", value<unsigned int>(),
        "the random number generator seed to use for this run")
        ;

    // By default this test should run on all available cores
    std::vector<std::string> cfg;
    cfg.push_back("hpx.os_threads=" +
        boost::lexical_cast<std::string>(hpx::threads::hardware_concurrency()));

    // Initialize and run HPX
    HPX_TEST_EQ_MSG(hpx::init(desc_commandline, argc, argv, cfg), 0,
        "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}
