//  Copyright (c) 2017 Taeguk Kwon
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/util/lightweight_test.hpp>

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#include "merge_tests.hpp"
#include "test_utils.hpp"

////////////////////////////////////////////////////////////////////////////
void merge_test()
{
    std::cout << "--- merge_test ---" << std::endl;
    test_merge<std::random_access_iterator_tag>();
    //test_merge<std::bidirectional_iterator_tag>();
    //test_merge<std::forward_iterator_tag>();
}

void merge_exception_test()
{
    std::cout << "--- merge_exception_test ---" << std::endl;
    test_merge_exception<std::random_access_iterator_tag>();
    //test_merge_exception<std::bidirectional_iterator_tag>();
    //test_merge_exception<std::forward_iterator_tag>();
}

void merge_bad_alloc_test()
{
    std::cout << "--- merge_bad_alloc_test ---" << std::endl;
    test_merge_bad_alloc<std::random_access_iterator_tag>();
    //test_merge_bad_alloc<std::bidirectional_iterator_tag>();
    //test_merge_bad_alloc<std::forward_iterator_tag>();
}

// This is not related to parallel::merge!!!
#include <hpx/exception_list.hpp>
#include <hpx/dataflow.hpp>
#include <hpx/lcos/future.hpp>
void test_for_issue_2816()
{
    try {
        hpx::future<void> left = hpx::lcos::make_ready_future();
        hpx::future<void> right = hpx::lcos::make_exceptional_future<void>(std::bad_alloc());
        hpx::future<void> f = hpx::dataflow(
            [](hpx::future<void> && left,
                hpx::future<void> && right) -> void
        {
            if (left.has_exception() || right.has_exception())
            {
                std::list<std::exception_ptr> errors;
                if (left.has_exception())
                    errors.push_back(left.get_exception_ptr());
                if (right.has_exception())
                    errors.push_back(right.get_exception_ptr());

                throw hpx::exception_list(std::move(errors));
            }
        }, std::move(left), std::move(right));
        f.get();
    }
    catch (std::bad_alloc const &) {
        std::cout << "std::bad_alloc !!" << std::endl;
        HPX_TEST(true);
    }
    catch (hpx::exception_list const& el) {
        std::cout << "hpx::exception_list !!" << std::endl;
        HPX_TEST(false);
    }
    catch (...) {
        HPX_TEST(false);
    }
}

///////////////////////////////////////////////////////////////////////////////
int hpx_main(boost::program_options::variables_map& vm)
{
    unsigned int seed = (unsigned int)std::time(nullptr);
    if (vm.count("seed"))
        seed = vm["seed"].as<unsigned int>();

    std::cout << "using seed: " << seed << std::endl;
    std::srand(seed);

    merge_test();
    merge_exception_test();
    merge_bad_alloc_test();
    test_for_issue_2816();

    std::cout << "Test Finish!" << std::endl;

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
    std::vector<std::string> const cfg = {
        "hpx.os_threads=all"
    };

    // Initialize and run HPX
    HPX_TEST_EQ_MSG(hpx::init(desc_commandline, argc, argv, cfg), 0,
        "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}
